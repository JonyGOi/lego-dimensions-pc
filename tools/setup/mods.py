"""Fase 64: prepara os mods do LEGO Dimensions para o port (rodado pelo launcher antes de abrir o jogo).

Tipos de mod aceitos (os mesmos de PS4 / shadPS4):
  - pacote da TT Games (.HDR + .DAT com nome proprio, ex. AllWorld.DAT feito com o BrickVault);
  - arquivos soltos no formato do /app0 (pasta com data\\ ou sce_sys\\, como a CUSA01176-mods do shadPS4).
Onde procura: <port>\\mods\\<cada pasta> (ordem alfabetica; quem esta no desligados.txt fica de fora; a primeira
vence), <jogo>-mods (shadPS4) e <jogo>\\Mods (onde alguns mods mandam extrair).

Por que nao basta montar o pacote do mod: o jogo so monta pacotes de nomes fixos (GAME, PATCH, STREAM...), e um
pacote PATCH a mais com arquivos repetidos derruba o jogo (eboot+0xcd5dc4) - os PATCH do jogo nunca repetem
arquivo entre si. Entao o mod entra DENTRO dos pacotes do jogo: para cada arquivo que o mod troca, a entrada dele
na tabela (.HDR) de todo pacote que o tem passa a apontar para uma copia dos bytes do mod, posta depois do fim do
.DAT. Os nomes, hashes e a quantidade de pacotes ficam como no PS4. O port le o manifesto e mostra ao jogo o .HDR
mudado e o .DAT com a cauda (o .DAT de verdade nunca e copiado nem alterado).

Saida em assets\\mods_cache: <PACOTE>.HDR (tabela mudada), <PACOTE>.tail (bytes dos mods), mods.txt (manifesto),
e os indices do radar em assets\\dat_index\\<PACOTE>_MODS.tsv.
Uso: mods.py --instalar MOD.zip --mods PASTA
     mods.py --root PORT --app0 JOGO [--patch PASTA] [--addcont PASTA] [--mods PASTA] [--cache PASTA] [--lingua en]
Linhas: MOD <nome>: <n> arquivos trocados, SOBREPOSICAO <pasta>, INSTALADO <pasta>, PRONTO <n>, AVISO ..., ERRO ...
"""
import argparse
import glob
import hashlib
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'nu2_shader_extractor'))
import nu2toc  # noqa: E402

GAME_PACK = re.compile(r'^(PATCHSTREAM|DLCSTREAM|STREAM|PATCH|GAME)\d*$', re.IGNORECASE)
LANG = 'pt'


def T(pt, en):
    return pt if LANG == 'pt' else en


def say(text):
    print(text, flush=True)


# ---------------------------------------------------------------- onde estao os mods
def overlay_root(folder):
    """Raiz no formato do /app0 dentro do mod: a propria pasta se tiver data\\ ou sce_sys\\, ou a unica subpasta que tiver."""
    def looks(p):
        return os.path.isdir(os.path.join(p, 'data')) or os.path.isdir(os.path.join(p, 'sce_sys'))
    if looks(folder):
        return folder
    subs = [os.path.join(folder, d) for d in os.listdir(folder) if os.path.isdir(os.path.join(folder, d))]
    return subs[0] if len(subs) == 1 and looks(subs[0]) else None


def find_packs(folder):
    packs = []
    base = folder.rstrip('\\/').count(os.sep)
    for root, dirs, files in os.walk(folder):
        if root.count(os.sep) - base >= 4:
            dirs[:] = []
        lower = {f.lower(): f for f in files}
        for f in files:
            stem, ext = os.path.splitext(f)
            if ext.lower() == '.hdr' and not GAME_PACK.match(stem) and (stem + '.dat').lower() in lower:
                packs.append(os.path.join(root, lower[(stem + '.dat').lower()]))
    return sorted(packs, key=lambda p: p.lower())


def mod_sources(mods_dir, app0):
    sources = []
    if os.path.isdir(mods_dir):
        off = set()
        list_file = os.path.join(mods_dir, 'desligados.txt')
        if os.path.exists(list_file):
            with open(list_file, encoding='utf-8', errors='replace') as f:
                off = {line.strip().lower() for line in f if line.strip() and not line.startswith('#')}
        for name in sorted(os.listdir(mods_dir), key=str.lower):
            path = os.path.join(mods_dir, name)
            if os.path.isdir(path) and name.lower() not in off:
                sources.append((name, path))
    if app0:
        shad = app0.rstrip('\\/') + '-mods'
        if os.path.isdir(shad):
            sources.append((os.path.basename(shad) + ' (shadPS4)', shad))
        for name in ('Mods', 'mods'):
            if os.path.isdir(os.path.join(app0, name)):
                sources.append((os.path.basename(app0.rstrip('\\/')) + '\\' + name, os.path.join(app0, name)))
                break
    return sources


# ---------------------------------------------------------------- tabela dos pacotes (formato novo, .CC4)
def toc_positions(block):
    """{nome em maiusculas: posicao da entrada no bloco}, id do pacote e versao, para mudar entradas no lugar."""
    archive_id, version, _, name_count, names_size = struct.unpack_from('>iiIII', block, 12)
    folders = [''] * name_count
    names = []
    p = 36 + names_size
    entry = 12 if version >= 2 else 10
    for i in range(name_count):
        name_off, parent = struct.unpack_from('>IH', block, p)
        is_file = struct.unpack_from('>H', block, p + entry - 2)[0]
        p += entry
        if name_off == 0xFFFFFFFF:
            continue
        text = folders[parent] + '\\' + nu2toc.cstr(block, 32 + name_off)
        if i == name_count - 1:
            is_file = len(names)
        if is_file:
            names.append(text)
        else:
            folders[i] = text
    p += 4
    file_count = struct.unpack_from('>I', block, p)[0]
    p += 4
    entries_at = p
    step = (8 if version >= 2 else 4) + 8 + (0 if archive_id <= -10 else 4)
    crc_at = entries_at + step * file_count
    # o mesmo casamento nome -> arquivo do nu2toc, guardando o indice
    marker = [(i, 0, 0, 0) for i in range(file_count)]
    rows = nu2toc.link_names(block, crc_at, file_count, names, marker)
    positions = {name.upper(): entries_at + step * offset for name, _, _, offset, _ in rows}
    return positions, archive_id, version


def toc_positions_old(hdr):
    """O mesmo para o formato antigo (GAME*, STREAM*): o .HDR e um u32 com o tamanho e a tabela do .DAT.
    Entradas de 16 bytes little-endian: offset>>8, tamanho guardado, tamanho real, (byte baixo do offset << 24) | metodo."""
    block = hdr[4:]
    archive_id, file_count = struct.unpack_from('<iI', block, 0)
    marker = [(i, 0, 0, 0) for i in range(file_count)]
    info = file_count * 16 + 8
    name_entries = struct.unpack_from('<I', block, info)[0]
    info += 4
    entry_size = 12 if archive_id <= -5 else 8
    names_at = info + name_entries * entry_size
    crc_at = struct.unpack_from('<I', block, names_at)[0] + names_at + 4
    names_at += 4
    paths, names, cursor = [], [], info
    for _ in range(file_count):
        text, piece, more = '', '', 1
        while more > 0:
            more, parent, name_off = struct.unpack_from('<hhi', block, cursor)
            cursor += entry_size
            piece = nu2toc.cstr(block, names_at + name_off) if name_off >= 0 else ''
            if parent != 0:
                text = paths[parent]
            paths.append(text)
            if more > 0 and piece:
                text = text + piece + '\\'
        names.append('\\' + text + piece)
    if archive_id == -1:
        index = {name.upper(): file_count - 1 - i for i, name in enumerate(names)}
    else:
        index = {name.upper(): offset for name, _, _, offset, _ in nu2toc.link_names(block, crc_at, file_count, names, marker)}
    return {name: 4 + 8 + 16 * i for name, i in index.items()}, archive_id, 0


def write_entry(block, pos, archive_id, version, offset, zsize, usize, packed):
    if block[4:8] != b'.CC4':  # formato antigo, little-endian
        if archive_id == -1:
            raise ValueError('pacote sem offsets deslocados')
        struct.pack_into('<IIII', block, pos, offset >> 8, zsize, usize, ((offset & 0xFF) << 24) | (2 if packed else 0))
        return
    if version < 2 or archive_id > -10:
        raise ValueError('formato de tabela desconhecido')
    if archive_id <= -13:
        struct.pack_into('>QII', block, pos, offset | ((2 << 56) if packed else 0), zsize, usize)
    else:
        struct.pack_into('>QII', block, pos, offset, zsize, usize | (0x80000000 if packed else 0))


# ---------------------------------------------------------------- pacotes do jogo

def game_packs(layers, addcont):
    """{PACOTE: (.HDR, .DAT) no host}: os do jogo (chunk1 tem os .HDR; os .DAT ficam nos chunkN; a camada de cima
    vence) e os dos DLCs (uma subpasta por DLC com DLCnn.HDR2 / DLCnn.DAT2, como a addcont do shadPS4)."""
    hdrs, dats = {}, {}
    for layer in reversed(layers):
        for p in glob.glob(os.path.join(layer, 'data', 'chunk*', '*.HDR')):
            hdrs[os.path.splitext(os.path.basename(p))[0].upper()] = p
        for p in glob.glob(os.path.join(layer, 'data', 'chunk*', '*.DAT')):
            dats[os.path.splitext(os.path.basename(p))[0].upper()] = p
    if addcont and os.path.isdir(addcont):
        for p in glob.glob(os.path.join(addcont, '*', '*.HDR2')):
            dat = p[:-5] + '.DAT2'
            if os.path.exists(dat):
                hdrs[os.path.splitext(os.path.basename(p))[0].upper()] = p
                dats[os.path.splitext(os.path.basename(p))[0].upper()] = dat
    return {k: (hdrs[k], dats[k]) for k in hdrs if k in dats}


# ---------------------------------------------------------------- principal
def prepare(root, app0, patch, mods_dir, cache, addcont):
    os.makedirs(cache, exist_ok=True)
    manifest = os.path.join(cache, 'mods.txt')
    sources = mod_sources(mods_dir, app0)
    overlays, mod_files = [], []  # mod_files: (mod, nome, bytes guardados, tamanho real, comprimido)
    stamp = hashlib.sha1()
    for label, folder in sources:
        over = overlay_root(folder)
        if over:
            overlays.append(over)
            say('SOBREPOSICAO %s' % over)
            for base, _, files in os.walk(over):
                for f in files:
                    st = os.stat(os.path.join(base, f))
                    stamp.update(('%s|%d|%d' % (os.path.join(base, f), st.st_size, st.st_mtime_ns)).encode('utf-8', 'replace'))
        for dat in find_packs(folder):
            st = os.stat(dat)
            stamp.update(('%s|%d|%d' % (dat, st.st_size, st.st_mtime_ns)).encode('utf-8', 'replace'))
            mod_files.append((label + ': ' + os.path.splitext(os.path.basename(dat))[0], dat))
        if not over and not find_packs(folder):
            say('AVISO ' + T('%s: nada que o port saiba carregar (nem pacote .HDR/.DAT, nem pastas data\\ ou sce_sys\\)' % label,
                             '%s: nothing the port can load (no .HDR/.DAT pack, no data\\ or sce_sys\\ folders)' % label))
    layers = overlays + ([patch] if patch and os.path.isdir(patch) else []) + [app0]
    packs = game_packs(layers, addcont)
    for name, (hdr_path, dat_path) in sorted(packs.items()):
        for path in (hdr_path, dat_path):
            st = os.stat(path)
            stamp.update(('%s|%d|%d' % (path, st.st_size, st.st_mtime_ns)).encode('utf-8', 'replace'))
    key = stamp.hexdigest()
    if os.path.exists(manifest):
        with open(manifest, encoding='utf-8') as f:
            if f.readline().strip() == '# chave ' + key:
                say(T('mods ja preparados (nada mudou)', 'mods already prepared (nothing changed)'))
                say('PRONTO %d' % len(sources))
                return 0
    for old in glob.glob(os.path.join(cache, '*.HDR')) + glob.glob(os.path.join(cache, '*.tail')):
        os.remove(old)

    # arquivos de cada pacote de mod; o primeiro mod que trouxer um nome vence
    wanted = {}
    for label, dat in mod_files:
        try:
            rows = nu2toc.read_toc(dat)
        except Exception as e:
            say('AVISO ' + T('%s: tabela do pacote ilegivel (%s)' % (label, e), '%s: unreadable pack table (%s)' % (label, e)))
            continue
        with open(dat, 'rb') as f:
            for name, usize, zsize, offset, packed in rows:
                if name.upper() in wanted:
                    continue
                f.seek(offset)
                wanted[name.upper()] = (label, name, f.read(zsize), usize, bool(packed))

    lines = ['# chave ' + key]
    lines += ['overlay\t%s' % o for o in overlays]
    found = set()
    for pack, (hdr_path, dat_path) in sorted(packs.items()):
        if not wanted:
            break
        with open(hdr_path, 'rb') as f:
            block = bytearray(f.read())
        try:
            positions, archive_id, version = (toc_positions if block[4:8] == b'.CC4' else toc_positions_old)(bytes(block))
        except Exception as e:
            say('AVISO ' + T('tabela de %s nao lida (%s)' % (pack, e), 'table of %s not read (%s)' % (pack, e)))
            continue
        hits = [n for n in wanted if n in positions]
        if not hits:
            continue
        base = os.path.getsize(dat_path)
        tail = bytearray()
        index_rows = []
        for n in hits:
            label, name, data, usize, packed = wanted[n]
            while len(tail) % 16:
                tail.append(0)
            write_entry(block, positions[n], archive_id, version, base + len(tail), len(data), usize, packed)
            index_rows.append((name, usize, len(data), len(tail), 2 if packed else 0))
            tail += data
            found.add(n)
        cache_hdr = os.path.join(cache, pack + '.HDR')
        cache_tail = os.path.join(cache, pack + '.tail')
        with open(cache_hdr, 'wb') as f:
            f.write(block)
        with open(cache_tail, 'wb') as f:
            f.write(tail)
        try:  # indice do radar (so para o log do port): falhar aqui nao impede os mods
            os.makedirs(os.path.join(root, 'assets', 'dat_index'), exist_ok=True)
            nu2toc.write_tsv(index_rows, os.path.join(root, 'assets', 'dat_index', pack + '_MODS.tsv'))
        except OSError:
            pass
        lines.append('pacote\t%s\t%s\t%s\t%s\t%d\t%s' % (pack, hdr_path, cache_hdr, dat_path, base, cache_tail))
    per_mod = {}
    for n, (label, name, *_rest) in wanted.items():
        hit = per_mod.setdefault(label, [0, 0])
        hit[0 if n in found else 1] += 1
    for label, (ok, new) in per_mod.items():
        say('MOD ' + T('%s: %d arquivo(s) do jogo trocado(s)' % (label, ok), '%s: %d game file(s) replaced' % (label, ok)))
        if new:
            say('AVISO ' + T('%s: %d arquivo(s) que o jogo nao tem ficaram de fora (o port so troca arquivos que ja existem)' % (label, new),
                             '%s: %d file(s) the game does not have were left out (the port only replaces existing files)' % (label, new)))
    with open(manifest, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')
    say('PRONTO %d' % len(sources))
    return 0


def install(zip_path, mods_dir):
    """Extrai um mod (.zip) em mods\\<nome do zip>; recusa caminhos que sairiam da pasta."""
    import zipfile
    name = re.sub(r'[<>:"/\\|?*]', '_', os.path.splitext(os.path.basename(zip_path))[0]).strip(' .') or 'mod'
    dest, n = os.path.join(mods_dir, name), 2
    while os.path.exists(dest):
        dest = os.path.join(mods_dir, '%s (%d)' % (name, n))
        n += 1
    with zipfile.ZipFile(zip_path) as z:
        for info in z.infolist():
            target = os.path.normpath(os.path.join(dest, info.filename))
            if os.path.isabs(info.filename) or not target.startswith(os.path.normpath(dest) + os.sep):
                say('ERRO ' + T('o zip tem um caminho perigoso (%s): nada foi instalado' % info.filename,
                                'the zip has an unsafe path (%s): nothing was installed' % info.filename))
                return 1
        os.makedirs(dest)
        z.extractall(dest)
    if not overlay_root(dest) and not find_packs(dest):
        say('AVISO ' + T('%s: o zip nao tem pacote .HDR/.DAT nem pastas data\\ ou sce_sys\\; confira se e um mod de PS4' % os.path.basename(dest),
                         '%s: the zip has no .HDR/.DAT pack and no data\\ or sce_sys\\ folders; check that it is a PS4 mod' % os.path.basename(dest)))
    say('INSTALADO ' + os.path.basename(dest))
    return 0


def main():
    global LANG
    parser = argparse.ArgumentParser(description='LEGO Dimensions PC: mods')
    parser.add_argument('--instalar', help='extrai um mod (.zip) na pasta de mods')
    parser.add_argument('--root')
    parser.add_argument('--app0')
    parser.add_argument('--patch', default='')
    parser.add_argument('--lingua', default='pt')
    parser.add_argument('--mods', help='pasta dos mods (padrao: <root>/mods)')
    parser.add_argument('--cache', help='saida (padrao: <root>/assets/mods_cache)')
    parser.add_argument('--addcont', default='', help='pasta das DLCs (uma subpasta por DLC)')
    args = parser.parse_args()
    LANG = 'en' if args.lingua.lower().startswith('en') else 'pt'
    try:
        if args.instalar:
            mods_dir = args.mods or os.path.join(args.root or '.', 'mods')
            os.makedirs(mods_dir, exist_ok=True)
            return install(os.path.abspath(args.instalar), os.path.abspath(mods_dir))
        if not args.root or not args.app0:
            parser.error('--root e --app0 sao necessarios')
        root = os.path.abspath(args.root)
        return prepare(root, os.path.abspath(args.app0), os.path.abspath(args.patch) if args.patch else '',
                       os.path.abspath(args.mods or os.path.join(root, 'mods')), os.path.abspath(args.cache or os.path.join(root, 'assets', 'mods_cache')),
                       os.path.abspath(args.addcont) if args.addcont else '')
    except OSError as e:
        say('ERRO %s' % e)
        return 1


if __name__ == '__main__':
    sys.exit(main())
