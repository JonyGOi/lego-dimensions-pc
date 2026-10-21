"""Leitura (so leitura) dos pacotes .DAT do jogo: arquivos .ps4_shaders.

O indice de cada pacote (nome, tamanho, tamanho empacotado, offset, metodo) vem de
tools/data/dat_index/<PACOTE>.tsv, gerado por tools/build_dat_index.py.

Compressao "DFLT" da TT: blocos [ 'DFLT' | u32 tamanho empacotado | u32 tamanho real | dados ].
Os dados sao deflate cru com uma diferenca: o tipo de bloco 00 quer dizer "Huffman dinamico" (no
deflate padrao, 00 e bloco sem compressao). Bloco com os dois tamanhos iguais vem sem compressao.
"""
import glob
import os
import struct
import zlib

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
DEFAULT_APP0 = r'C:\Jogos\CUSA01176'
# Fase 47: DLCs (uma pasta por DLC com DLCnn.DAT2 + DLCnn.HDR2), a mesma pasta de addcont do shadPS4
DEFAULT_ADDCONT = os.path.join(os.environ.get('APPDATA', ''), 'shadPS4', 'addcont', 'CUSA01176')
INDEX_DIR = os.path.join(ROOT, 'tools', 'data', 'dat_index')


def find_patch(app0):
    """Fase 58: atualizacao numa pasta separada (formato do shadPS4: CUSA01176-patch), dentro da pasta do jogo
    ou ao lado dela. '' = nenhuma."""
    app0 = os.path.normpath(app0)
    name = os.path.basename(app0)
    parent = os.path.dirname(app0)
    for d in (os.path.join(app0, name + '-patch'), os.path.join(parent, name + '-patch'),
              os.path.join(app0, 'CUSA01176-patch'), os.path.join(parent, 'CUSA01176-patch')):
        if os.path.isfile(os.path.join(d, 'eboot.bin')) or os.path.isfile(os.path.join(d, 'sce_sys', 'param.sfo')):
            return d
    return ''


def find_dats(app0, addcont=None, patch=None):
    dats = {os.path.basename(p).upper()[:-4]: p for p in glob.glob(os.path.join(app0, 'data', 'chunk*', '*.DAT'))}
    patch = find_patch(app0) if patch is None else patch
    if patch and os.path.isdir(patch):  # Fase 58: os .DAT da atualizacao vencem os de mesmo nome do jogo base
        dats.update({os.path.basename(p).upper()[:-4]: p for p in glob.glob(os.path.join(patch, 'data', 'chunk*', '*.DAT'))})
    if addcont and os.path.isdir(addcont):  # Fase 47: pacotes dos DLCs (o indice vem do mesmo build_dat_index.py)
        dats.update({os.path.basename(p).upper()[:-5]: p for p in glob.glob(os.path.join(addcont, '*', '*.DAT2'))})
    return dats


def unpack(raw, size):
    out = bytearray()
    pos = 0
    while pos < len(raw) and len(out) < size:
        magic = raw[pos:pos + 4]
        packed, real = struct.unpack_from('<II', raw, pos + 4)
        body = raw[pos + 12:pos + 12 + packed]
        if magic != b'DFLT':
            raise ValueError('bloco desconhecido %r' % magic)
        if packed == real:
            out += body
        else:
            first = body[0] | (0 if body[0] & 6 else 4)  # tipo 00 da TT -> 10 (dinamico) do deflate
            out += zlib.decompress(bytes([first]) + body[1:], -15)
        pos += 12 + packed
    return bytes(out)


def read_tsv(path, suffix):
    rows = []
    if os.path.exists(path):
        with open(path, encoding='latin-1') as f:
            for line in f:
                p = line.rstrip('\n').split('\t')
                if len(p) >= 5 and p[0].lower().endswith(suffix):
                    rows.append((p[0], int(p[1]), int(p[2]), int(p[3]), int(p[4])))
    return rows


# Fase 60: indice de cada pacote tirado da tabela do proprio .DAT de quem prepara (o que vem em
# tools/data/dat_index foi gerado de um dump so, e um PATCH.DAT de outra origem nao batia com ele).
# Fica guardado em assets/dat_index/<PACOTE>.tsv, refeito quando o .DAT muda (tamanho ou data).
LOCAL_INDEX_DIR = os.path.join(ROOT, 'assets', 'dat_index')


def local_index(pack, dat_path):
    path = os.path.join(LOCAL_INDEX_DIR, pack + '.tsv')
    stamp = '%d %d' % (os.path.getsize(dat_path), int(os.path.getmtime(dat_path)))
    try:
        if open(path + '.carimbo', encoding='ascii').read() == stamp:
            return path
    except OSError:
        pass
    import nu2toc
    rows = nu2toc.read_toc(dat_path)
    if not rows:
        raise ValueError('tabela vazia')
    os.makedirs(LOCAL_INDEX_DIR, exist_ok=True)
    nu2toc.write_tsv(rows, path)
    with open(path + '.carimbo', 'w', encoding='ascii') as f:
        f.write(stamp)
    return path


def entries(pack, suffix='.ps4_shaders', dat_path=None):
    """Arquivos do pacote com o sufixo: os da tabela do proprio .DAT (se der para ler) e, para o que ela nao
    tiver, os do indice que vem com o port."""
    shipped = read_tsv(os.path.join(INDEX_DIR, pack + '.tsv'), suffix)
    own = []
    if dat_path:
        try:
            own = read_tsv(local_index(pack, dat_path), suffix)
        except Exception as e:  # formato inesperado: fica o indice que vem com o port, como antes
            print('AVISO: tabela de %s nao lida (%s): usando o indice que vem com o port' % (pack, e), flush=True)
    seen = {row[0] for row in own}
    yield from own
    yield from (row for row in shipped if row[0] not in seen)


def iter_files(app0, suffix='.ps4_shaders', addcont=None, errors=None, patch=None):
    """(pacote, nome, conteudo descompactado) de cada arquivo com o sufixo. `errors` (Counter): conta
    por pacote os arquivos que nao se descompactam (indice de outra versao do jogo) em vez de parar."""
    dats = find_dats(app0, addcont, patch)
    for pack in sorted(dats):
        size_on_disk = os.path.getsize(dats[pack])
        with open(dats[pack], 'rb') as f:  # so leitura
            for name, size, packed, offset, method in entries(pack, suffix, dats[pack]):
                if offset + packed > size_on_disk:
                    if errors is None:
                        raise ValueError('%s: %s fora do pacote' % (pack, name))
                    errors[pack] += 1
                    continue
                f.seek(offset)
                raw = f.read(packed)
                try:
                    data = raw if method == 0 else unpack(raw, size)
                except Exception:
                    if errors is None:
                        raise
                    errors[pack] += 1
                    continue
                yield pack, name, data
