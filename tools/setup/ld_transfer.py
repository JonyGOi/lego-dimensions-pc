"""LD PC Transfer (Fase 63): converte saves do LEGO Dimensions entre as plataformas.

Formato de cada arquivo (descoberto no eboot do PS4, gravacao em 0x54c2d0 e leitura em 0x54b660):
  cabecalho de 24 bytes na ordem de bytes do console:
    0x00 versao do cabecalho (13; Wii U 12)
    0x04 ~FNV-1 (inicio 0xffffffff) dos 4 bytes da versao, como estao no arquivo
    0x08 float (sempre 0) e 0x0c o mesmo hash dos 4 bytes dele (0x502c3f10)
    0x10 ~FNV-1 do payload (no GAME e o do V2GAME: o GAME e so o comeco do mesmo payload)
    0x14 tamanho do payload
  payload, little-endian em todas as plataformas (fora os pedacos de NATIVE, ver abaixo):
    zeros (preambulo: 0x440 no PS4 de 64 bits, 0x438 nos consoles de 32 bits; 8 nos DLC)
    marcador 0x4dee7e53, versao do bloco (GAME 28 / Wii U 27; DLC 21 / Wii U 20)
    pedacos [tag][tamanho][dados][tag], sentinela 0x075bcd15, zeros ate o tamanho fixo da plataforma
  Da versao 27 para a 28 so entrou o pedaco 0x5081eb33 no GAME; da 20 para a 21 so o 0x10ca236c no DLC
  (os dois logo depois do 0xd02558b0). O PS4 nao aceita GAME v27 (comeca historia nova): o pedaco entra zerado.
  Os arquivos de opcoes (OPTS, OPTSC, FEOPTS) sao estruturas cruas diferentes por console: nao sao
  convertidos (o jogo cria os padroes).

Uso: ld_transfer.py --detectar PASTA
     ld_transfer.py --origem PASTA --destino PASTA --formato ps4|x360|wiiu|ps3 [--backup PASTA] [--lingua en]
Saida por linha: ORIGEM <formato> <versao> <dlcs>, BACKUP <pasta>, GRAVADO <arquivo>, OK <n>, ERRO <texto>.
"""
import argparse
import datetime
import os
import re
import shutil
import struct
import sys

MARK = 0x4DEE7E53
SENTINEL = 0x075BCD15
NEW_MAIN_CHUNK = 0x5081EB33  # versao 28 do GAME
NEW_DLC_CHUNK = 0x10CA236C  # versao 21 do DLC
ANCHOR_CHUNK = 0xD02558B0  # os pedacos novos vem logo depois deste
DLC_INDICES = [k for k in range(1, 32) if k != 15]  # numeracao do PS4 (DLC01..DLC31, sem o 15)


class Format:
    def __init__(self, key, endian, header_version, main_block, dlc_block, preamble, game_size, v2_size, dlc_size, suffix,
                 dlc_offset, split=True, align=0):
        self.key, self.endian, self.header_version = key, endian, header_version
        self.main_block, self.dlc_block, self.preamble = main_block, dlc_block, preamble
        self.game_size, self.v2_size, self.dlc_size = game_size, v2_size, dlc_size
        self.suffix, self.dlc_offset, self.split, self.align = suffix, dlc_offset, split, align

    def dlc_name(self, k):
        n = k - self.dlc_offset
        return 'DLC%d' % n if self.suffix == '0' else 'DLC%02d' % n

    def main_names(self):
        if self.split:
            return 'V2GAME' + self.suffix, 'GAME' + self.suffix
        return 'GAME' + self.suffix, None


# tamanhos de payload medidos em saves reais de cada plataforma (o jogo grava buffers de tamanho fixo)
FORMATS = {
    'ps4': Format('ps4', '<', 13, 28, 21, 0x440, 100425, 308053, 234817, '01', 0),
    'x360': Format('x360', '>', 13, 28, 21, 0x438, 100417, 308045, 234817, '01', 0),
    'wiiu': Format('wiiu', '>', 12, 27, 20, 0x438, 100381, 308809, 234805, '0', 1),
    # PS3: um GAME01 so (sem V2GAME), arquivos em blocos de 1 KiB; precisa de save descriptografado (RPCS3)
    'ps3': Format('ps3', '>', 13, 28, 21, 0x438, None, 308200, 235496, '01', 0, split=False, align=1024),
}

MESSAGES = {
    'pt': {
        'no_main': 'nao achei GAME / V2GAME do LEGO Dimensions em %s',
        'many_slots': 'essa pasta tem mais de um save; escolha um deles: %s',
        'old_layout': '%s e de uma versao do jogo sem as atualizacoes: atualize o jogo, carregue e salve de novo antes de converter',
        'bad_header': '%s nao e um save do LEGO Dimensions (ou esta criptografado: saves de PS3 de console precisam ser descriptografados antes)',
        'bad_hash': '%s esta corrompido (soma de verificacao nao bate)',
        'bad_body': '%s: estrutura do save nao reconhecida',
        'too_new': '%s e de uma versao mais nova do jogo (bloco v%d) do que o destino aceita (v%d)',
        'too_old': '%s e de uma versao antiga demais do jogo (bloco v%d; o destino usa v%d): atualize o jogo de origem e salve de novo',
        'too_big': 'o save nao cabe no formato de destino (%s)',
        'same_dir': 'a pasta de destino nao pode ser a mesma da origem',
    },
    'en': {
        'no_main': 'no LEGO Dimensions GAME / V2GAME found in %s',
        'many_slots': 'that folder holds more than one save; pick one of them: %s',
        'old_layout': '%s comes from a game version without the updates: update the game, load and save again before converting',
        'bad_header': '%s is not a LEGO Dimensions save (or it is encrypted: PS3 console saves must be decrypted first)',
        'bad_hash': '%s is corrupted (checksum mismatch)',
        'bad_body': '%s: unrecognised save structure',
        'too_new': '%s comes from a newer game version (block v%d) than the target accepts (v%d)',
        'too_old': '%s comes from a game version that is too old (block v%d; the target uses v%d): update the source game and save again',
        'too_big': 'the save does not fit the target format (%s)',
        'same_dir': 'the destination folder cannot be the source folder',
    },
}
LANG = 'en'


class TransferError(Exception):
    pass


def fail(key, *args):
    raise TransferError(MESSAGES[LANG][key] % args)


def fnv(data):
    h = 0xFFFFFFFF
    for b in data:
        h = ((h * 0x01000193) & 0xFFFFFFFF) ^ b
    return (~h) & 0xFFFFFFFF


def read_file(path):
    """Devolve (ordem de bytes, versao do cabecalho, hash guardado, payload)."""
    with open(path, 'rb') as f:
        data = f.read()
    if len(data) < 24:
        fail('bad_header', os.path.basename(path))
    endian = '<' if struct.unpack_from('<I', data, 0)[0] < 0x100 else '>'
    version, magic, _, _, stored, size = struct.unpack_from(endian + 'IIIIII', data, 0)
    if magic != fnv(data[0:4]):
        fail('bad_header', os.path.basename(path))
    if version < 12:
        fail('old_layout', os.path.basename(path))  # Wii U sem atualizacao: varios saves num GAME0 so
    if size > len(data) - 24:
        fail('bad_header', os.path.basename(path))
    return endian, version, stored, data[24:24 + size]


def parse_body(payload, name):
    """Payload -> (versao do bloco, [(tag, bytes do pedaco inteiro)])."""
    start = payload.find(struct.pack('<I', MARK))
    if start < 0 or any(payload[:start]):
        fail('bad_body', name)
    version = struct.unpack_from('<I', payload, start + 4)[0]
    pos, chunks = start + 8, []
    while True:
        if pos + 4 > len(payload):
            fail('bad_body', name)
        tag = struct.unpack_from('<I', payload, pos)[0]
        if tag == SENTINEL:
            break
        if pos + 12 > len(payload):
            fail('bad_body', name)
        size = struct.unpack_from('<I', payload, pos + 4)[0]
        stop = pos + 8 + size
        if stop + 4 > len(payload) or struct.unpack_from('<I', payload, stop)[0] != tag:
            fail('bad_body', name)
        chunks.append((tag, payload[pos:stop + 4]))
        pos = stop + 4
    if any(payload[pos + 4:]):
        fail('bad_body', name)
    return version, chunks


def adapt(version, chunks, target_version, new_chunk, new_size, upgrade, name):
    """Leva o bloco para a versao do destino, tirando ou pondo o pedaco que mudou entre elas.

    DLC de versao anterior fica como esta (saves reais do PS4 tem DLC v20 que o jogo nunca regravou). Para o GAME
    o pedaco novo entra zerado logo depois do 0xd02558b0, onde ele fica nos saves de verdade (6 contadores, zerados
    num jogo novo)."""
    if version == target_version or (version < target_version and not upgrade):
        return version, chunks
    if version == target_version + 1:
        return target_version, [c for c in chunks if c[0] != new_chunk]
    if version == target_version - 1:
        anchor = [i for i, c in enumerate(chunks) if c[0] == ANCHOR_CHUNK]
        if not anchor:
            fail('bad_body', name)
        chunk = struct.pack('<II', new_chunk, new_size) + b'\0' * new_size + struct.pack('<I', new_chunk)
        return target_version, chunks[:anchor[0] + 1] + [(new_chunk, chunk)] + chunks[anchor[0] + 1:]
    fail('too_new' if version > target_version else 'too_old', name, version, target_version)


def build_payload(fmt, version, chunks, size, preamble, name):
    body = struct.pack('<II', MARK, version) + b''.join(c[1] for c in chunks) + struct.pack('<I', SENTINEL)
    if preamble + len(body) > size:
        fail('too_big', name)
    return b'\0' * preamble + body + b'\0' * (size - preamble - len(body))


def header(fmt, payload_hash, size):
    e = fmt.endian
    version = struct.pack(e + 'I', fmt.header_version)
    zero = b'\0\0\0\0'
    return version + struct.pack(e + 'IIIII', fnv(version), 0, fnv(zero), payload_hash, size)


def write_file(path, data, align):
    if align and len(data) % align:
        data += b'\0' * (align - len(data) % align)
    tmp = path + '.novo'
    with open(tmp, 'wb') as f:
        f.write(data)
    os.replace(tmp, path)


def find_ci(folder, name):
    for entry in os.listdir(folder):
        if entry.lower() == name.lower() and os.path.isfile(os.path.join(folder, entry)):
            return os.path.join(folder, entry)
    return None


def has_save(folder):
    return any(find_ci(folder, n) for n in ('V2GAME01', 'V2GAME0', 'GAME01', 'GAME0'))


def find_slot(folder):
    """Aceita a pasta do save ou uma pasta acima (savedata/CUSA01176, user/80000001...) com um save so dentro."""
    if not os.path.isdir(folder) or has_save(folder):
        return folder
    found = []
    for root, dirs, _ in os.walk(folder):
        if root.count(os.sep) - folder.rstrip(os.sep).count(os.sep) > 4:
            dirs[:] = []
            continue
        if root != folder and has_save(root):
            found.append(root)
            dirs[:] = []
    if len(found) > 1:
        fail('many_slots', ', '.join(os.path.relpath(f, folder) for f in sorted(found)))
    return found[0] if found else folder


def detect(folder):
    """Le a pasta de origem: formato, versao do bloco, payload do GAME completo e {indice canonico: payload de DLC}."""
    folder = find_slot(folder)
    main = find_ci(folder, 'V2GAME01') or find_ci(folder, 'V2GAME0')
    split = main is not None
    if not main:
        main = find_ci(folder, 'GAME01') or find_ci(folder, 'GAME0')
    if not main:
        fail('no_main', folder)
    endian, header_version, stored, payload = read_file(main)
    if fnv(payload) != stored:
        fail('bad_hash', os.path.basename(main))
    if endian == '<':
        key = 'ps4'
    elif header_version <= 12:
        key = 'wiiu'
    else:
        key = 'x360' if split else 'ps3'
    version, chunks = parse_body(payload, os.path.basename(main))
    src = FORMATS[key]
    dlcs = {}
    for k in DLC_INDICES:
        path = find_ci(folder, src.dlc_name(k))
        if not path:
            continue
        _, _, dlc_stored, dlc_payload = read_file(path)
        if fnv(dlc_payload) != dlc_stored:
            fail('bad_hash', os.path.basename(path))
        dlcs[k] = parse_body(dlc_payload, os.path.basename(path))
    return key, (version, chunks), dlcs


SAVE_NAME = re.compile(r'^(V2GAME|GAME|DLC|Z?OPTSC?|FEOPTS)\d+$', re.IGNORECASE)


def backup_existing(dest, backup_root=None):
    """Move os saves que ja estao no destino para uma pasta de backup (nada e apagado). Sem backup_root ela fica ao
    lado do destino; o LD PC Transfer passa uma pasta propria, porque o port e o shadPS4 listariam a do lado como save."""
    if not os.path.isdir(dest):
        return None
    old = [e for e in os.listdir(dest) if SAVE_NAME.match(e) and os.path.isfile(os.path.join(dest, e))]
    if not old:
        return None
    stamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
    if backup_root:
        backup = os.path.join(backup_root, stamp + '_' + os.path.basename(os.path.normpath(dest)))
    else:
        backup = os.path.normpath(dest) + '_antes_ldtransfer_' + stamp
    os.makedirs(backup)
    for e in old:
        shutil.move(os.path.join(dest, e), os.path.join(backup, e))
    return backup


def swap_words(data):
    return b''.join(data[i:i + 4][::-1] for i in range(0, len(data), 4))


def pairs_to_be(data):
    """[A][B] + (indice, valor)... do PS4 -> [B em big-endian][A] + (valor, indice)... dos consoles big-endian."""
    units = [data[i + 4:i + 8] + data[i:i + 4] for i in range(0, len(data), 8)]
    units[0] = units[0][:4][::-1] + units[0][4:]
    return b''.join(units)


def pairs_to_le(data):
    units = [data[i:i + 8] for i in range(0, len(data), 8)]
    units[0] = units[0][:4][::-1] + units[0][4:]
    return b''.join(u[4:] + u[:4] for u in units)


def swap_record_values(data):
    """Registros de 24 bytes [dono][propriedade][valor u64][padrao u64]: os consoles big-endian gravam cada u64
    como duas palavras little-endian na ordem da memoria deles (metade alta primeiro)."""
    out = bytearray(data)
    for i in range(0, len(data) - 23, 24):
        out[i + 8:i + 16] = data[i + 12:i + 16] + data[i + 8:i + 12]
        out[i + 16:i + 24] = data[i + 20:i + 24] + data[i + 16:i + 20]
    return bytes(out)


# pedacos com partes cruas na ordem de bytes do console (achados comparando o save do Wii U com o do PS4 e
# testados no port): o resto do payload e little-endian em toda plataforma
NATIVE = {
    0x97ACD358: (swap_words, swap_words),  # mapa de bits em palavras de 32 bits
    0x38EFEFE9: (pairs_to_be, pairs_to_le),  # lista de pares de 32 bits
}
for _tag in (0x35EEAB6F, 0x00BD2EA0, 0x29672BE1, 0x2DC598C5, 0x0ABBE129, 0xEF3A5C65, 0xD4AAD89E, 0x1BBDBDF5,
             0x774D4F35, 0x331B6C77, 0xCA088DCC, 0x646F2CD1, 0x73ED76F1, 0x60AD7021, 0xD02558B0):
    NATIVE[_tag] = (swap_record_values, swap_record_values)  # listas de registros com valores de 64 bits


def reorder(chunks, src_endian, dst_endian):
    if src_endian == dst_endian:
        return chunks
    out = []
    for tag, chunk in chunks:
        if tag in NATIVE:
            data = chunk[8:-4]
            data = NATIVE[tag][1 if dst_endian == '<' else 0](data)
            chunk = chunk[:8] + data + chunk[-4:]
        out.append((tag, chunk))
    return out


def convert(origin, dest, target_key, backup_root=None):
    if os.path.normcase(os.path.abspath(origin)) == os.path.normcase(os.path.abspath(dest)):
        fail('same_dir')
    key, (version, chunks), dlcs = detect(origin)
    print('ORIGEM %s %d %d' % (key, version, len(dlcs)), flush=True)
    fmt = FORMATS[target_key]
    src_endian = FORMATS[key].endian
    v2_name, game_name = fmt.main_names()
    chunks = reorder(chunks, src_endian, fmt.endian)
    version, chunks = adapt(version, chunks, fmt.main_block, NEW_MAIN_CHUNK, 24, True, v2_name)
    main = build_payload(fmt, version, chunks, fmt.v2_size, fmt.preamble, v2_name)
    outputs = [(v2_name, header(fmt, fnv(main), len(main)) + main)]
    if game_name:
        # o GAME e o comeco do mesmo payload, com o hash do V2GAME no cabecalho
        outputs.append((game_name, header(fmt, fnv(main), fmt.game_size) + main[:fmt.game_size]))
    for k, (dlc_version, dlc_chunks) in sorted(dlcs.items()):
        name = fmt.dlc_name(k)
        dlc_chunks = reorder(dlc_chunks, src_endian, fmt.endian)
        dlc_version, dlc_chunks = adapt(dlc_version, dlc_chunks, fmt.dlc_block, NEW_DLC_CHUNK, 0, False, name)
        payload = build_payload(fmt, dlc_version, dlc_chunks, fmt.dlc_size, 8, name)
        outputs.append((name, header(fmt, fnv(payload), len(payload)) + payload))
    os.makedirs(dest, exist_ok=True)
    backup = backup_existing(dest, backup_root)
    for name, data in outputs:
        write_file(os.path.join(dest, name), data, fmt.align)
    # so avisa depois de gravar tudo: quem le a saida pode fechar o cano sem deixar o save pela metade
    if backup:
        print('BACKUP %s' % backup, flush=True)
    for name, _ in outputs:
        print('GRAVADO %s' % name, flush=True)
    print('OK %d' % len(outputs), flush=True)


def main():
    global LANG
    parser = argparse.ArgumentParser(description='LD PC Transfer')
    parser.add_argument('--detectar')
    parser.add_argument('--origem')
    parser.add_argument('--destino')
    parser.add_argument('--formato', choices=sorted(FORMATS))
    parser.add_argument('--lingua', default='en')
    parser.add_argument('--backup', help='folder for backing up saves already present at destination')
    args = parser.parse_args()
    LANG = 'en' if args.lingua.lower().startswith('en') else 'pt'
    try:
        if args.detectar:
            key, (version, _), dlcs = detect(args.detectar)
            print('ORIGEM %s %d %d' % (key, version, len(dlcs)), flush=True)
        elif args.origem and args.destino and args.formato:
            convert(args.origem, args.destino, args.formato, args.backup)
        else:
            parser.print_usage()
            return 1
    except (TransferError, OSError) as e:
        print('ERRO %s' % e, flush=True)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
