"""Fase 60: tabela de arquivos (nome -> offset / tamanho / compressao) lida do proprio pacote .DAT do jogo.

O indice em tools/data/dat_index foi gerado de um dump so; um PATCH.DAT de outra origem (sem mod, ou com
outro mod) guarda os arquivos em outras posicoes, e o extrator nao conseguia ler os shaders dele. Este
leitor monta o mesmo indice a partir do pacote de quem esta preparando o port. So LE o pacote.

Cabecalho do pacote: u32 LE com a posicao (codificada) do bloco de tabela e u32 LE com o tamanho dele.
Dois formatos de tabela da TT Games:
  - novo (".CC4" em +4; campos big-endian): pastas e nomes numa arvore, lista de arquivos, hashes;
  - antigo (campos little-endian): lista de arquivos de 16 bytes, arvore de nomes, hashes.
Em ambos, cada nome chega ao seu arquivo pelo hash FNV-1a do caminho (sem a barra inicial, em
maiusculas). A TT Games usa o primo 1677619 nos hashes de 32 bits; a variante (32/64 bits, ordem dos
bytes, primo) e escolhida pela que mais acerta. Conferido contra o indice antigo dos pacotes do jogo.
"""
import os
import struct

VARIANTS = (  # (bits, big endian, primo)
    (32, True, 1677619), (32, False, 1677619), (32, True, 16777619), (32, False, 16777619),
    (64, True, 1099511628211), (64, False, 1099511628211),
)


def header_offset(first, archive_size):
    value = first
    if value & 0x80000000:
        value = (((value ^ 0xFFFFFFFF) << 8) + 256) & 0xFFFFFFFF
    if archive_size > 0xFFFFFFFF:
        value = 0x100000000 ^ value
    return value


def fnv(path, bits, prime):
    mask = (1 << bits) - 1
    h = 0xCBF29CE484222325 if bits == 64 else 0x811C9DC5
    for ch in path.upper():
        h = ((h ^ ord(ch)) * prime) & mask
    return h


def cstr(block, at):
    return block[at:block.index(b'\0', at)].decode('latin-1')


def link_names(block, crc_at, file_count, names, entries, tree=None, fixed=None):
    """Liga cada nome ("\\pasta\\arquivo") ao arquivo pelo hash; escolhe a variante que mais acerta. tree: nome ->
    indice do arquivo no no da arvore (tabelas .CC4), que vale para os arquivos de hash 0; fixed recebe os nomes em
    que a lista por extenso discordava da arvore."""
    best = None
    sample = names[:400]
    for bits, big, prime in VARIANTS:
        step = bits // 8
        if crc_at + step * file_count > len(block):
            continue
        fmt = ('>' if big else '<') + ('Q' if bits == 64 else 'I')
        table = {struct.unpack_from(fmt, block, crc_at + step * i)[0]: i for i in range(file_count)}
        hits = sum(1 for n in sample if fnv(n[1:], bits, prime) in table)
        if best is None or hits > best[0]:
            best = (hits, table, bits, prime, big)
    out = []
    if not best or not best[0]:
        return out
    _, table, bits, prime, big = best
    # hashes repetidos: os arquivos com hash 0 sao os primeiros da lista, e os caminhos deles vem por
    # extenso logo depois da tabela de hashes (contagem, um u32, e os nomes terminados em zero)
    explicit = {}
    if 0 in table:
        p = crc_at + (bits // 8) * file_count
        count = struct.unpack_from('>I' if big else '<I', block, p)[0] if p + 8 <= len(block) else 0
        p += 8
        for j in range(min(count, file_count)):
            if p >= len(block):
                break
            path = cstr(block, p)
            p += len(path) + 1
            while p < len(block) and block[p] == 0:
                p += 1
            explicit[path.lstrip('\\').upper()] = j
    # Issue #4: pacotes de mod (ex. o LORDVORTECH de connorh315) trazem essa lista na ordem do jogo, mas com os
    # arquivos de hash 0 em outra ordem; o jogo segue a arvore de nomes, entao ela vence quando aponta para um deles.
    zero = set()
    if explicit and tree:
        fmt = ('>' if big else '<') + ('Q' if bits == 64 else 'I')
        zero = {i for i in range(min(file_count, len(explicit) + 8)) if struct.unpack_from(fmt, block, crc_at + (bits // 8) * i)[0] == 0}
    for name in names:
        index = explicit.get(name[1:].upper())
        if index is not None and tree and tree.get(name.upper()) in zero and tree[name.upper()] != index:
            index = tree[name.upper()]
            if fixed is not None:
                fixed.append(name)
        if index is None:
            index = table.get(fnv(name[1:], bits, prime))
        if index is not None:
            offset, zsize, usize, packed = entries[index]
            out.append((name, usize, zsize, offset, packed))
    return out



def read_new(block, fixed=None):
    archive_id, version, _, name_count, names_size = struct.unpack_from('>iiIII', block, 12)
    folders = [''] * name_count
    names = []
    tree = {}  # nome -> indice do arquivo (+6 do no; so na versao 2)
    p = 36 + names_size
    entry = 12 if version >= 2 else 10
    for i in range(name_count):
        name_off, parent = struct.unpack_from('>IH', block, p)
        is_file = struct.unpack_from('>H', block, p + entry - 2)[0]
        file_index = struct.unpack_from('>H', block, p + 6)[0] if version >= 2 else None
        p += entry
        if name_off == 0xFFFFFFFF:
            continue
        text = folders[parent] + '\\' + cstr(block, 32 + name_off)
        if i == name_count - 1:
            is_file = len(names)
        if is_file:
            names.append(text)
            if file_index is not None:
                tree[text.upper()] = file_index
        else:
            folders[i] = text
    p += 4
    file_count = struct.unpack_from('>I', block, p)[0]
    p += 4
    entries = []
    for _ in range(file_count):
        if version >= 2:
            offset = struct.unpack_from('>Q', block, p)[0]
            p += 8
        else:
            offset = struct.unpack_from('>I', block, p)[0]
            p += 4
        zsize, usize = struct.unpack_from('>II', block, p)
        p += 8
        packed = 0
        if archive_id <= -13:
            packed = 2 if offset >> 56 else 0
            offset &= 0xFFFFFFFFFFFFFF
        elif archive_id <= -10:
            packed = 2 if usize >> 31 else 0
            usize &= 0x7FFFFFFF
        else:
            packed = block[p]
            low = block[p + 3]
            p += 4
            offset = (offset << 8) | low
        entries.append((offset, zsize, usize, packed))
    return link_names(block, p, file_count, names, entries, tree, fixed)


def read_old(block):
    archive_id, file_count = struct.unpack_from('<iI', block, 0)
    entries = []
    for i in range(file_count):
        offset, zsize, usize, extra = struct.unpack_from('<IIII', block, 8 + 16 * i)
        if archive_id != -1:
            offset <<= 8
        entries.append((offset + (extra >> 24), zsize, usize, extra & 0xFFFFFF))
    info = file_count * 16 + 8
    name_entries = struct.unpack_from('<I', block, info)[0]
    info += 4
    entry_size = 12 if archive_id <= -5 else 8
    names_at = info + name_entries * entry_size
    crc_at = struct.unpack_from('<I', block, names_at)[0]
    names_at += 4
    crc_at += names_at
    # arvore de nomes: percorrida em sequencia; cada arquivo termina numa entrada com "proximo" <= 0
    paths = []
    names = []
    cursor = info
    for _ in range(file_count):
        text = ''
        piece = ''
        more = 1
        while more > 0:
            more, parent, name_off = struct.unpack_from('<hhi', block, cursor)
            cursor += entry_size
            piece = cstr(block, names_at + name_off) if name_off >= 0 else ''
            if parent != 0:
                text = paths[parent]
            paths.append(text)
            if more > 0 and piece:
                text = text + piece + '\\'
        names.append('\\' + text + piece)
    if archive_id == -1:  # sem hashes: os nomes vem na ordem inversa dos arquivos
        rows = []
        for i, name in enumerate(names):
            offset, zsize, usize, packed = entries[file_count - 1 - i]
            rows.append((name, usize, zsize, offset, packed))
        return rows
    return link_names(block, crc_at, file_count, names, entries)


def read_toc(path, fixed=None):
    """Lista de (nome, tamanho, tamanho_no_pacote, offset, metodo), como as linhas de dat_index/*.tsv. fixed: lista
    que recebe os arquivos de hash 0 cuja lista por extenso discordava da arvore de nomes (ver link_names)."""
    size = os.path.getsize(path)
    with open(path, 'rb') as f:  # so leitura
        first, hdr_size = struct.unpack('<II', f.read(8))
        f.seek(header_offset(first, size))
        block = f.read(hdr_size)
    if block[4:8] == b'.CC4':
        return read_new(block, fixed)
    return read_old(block)


def write_tsv(rows, out_path):
    with open(out_path, 'w', encoding='latin-1', errors='replace', newline='\n') as f:
        for name, usize, zsize, offset, packed in rows:
            f.write('%s\t%d\t%d\t%d\t%d\n' % (name, usize, zsize, offset, packed))
