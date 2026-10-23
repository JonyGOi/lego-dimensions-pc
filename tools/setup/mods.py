"""Fase 64: prepara os mods do LEGO Dimensions para o port (rodado pelo launcher antes de abrir o jogo).

Tipos de mod aceitos (os mesmos de PS4 / shadPS4):
  - pacote da TT Games (.HDR + .DAT com nome proprio, ex. AllWorld.DAT feito com o BrickVault);
  - pacote do jogo inteiro editado (ex. PATCH.HDR + PATCH.DAT soltos, como os mods de shadPS4): entram so os arquivos
    que diferem do pacote de mesmo nome do jogo;
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
Uso: mods.py --instalar MOD.zip|.rar|.7z --mods PASTA
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
# Dados de build de cada pacote (idiomas, SKU, versao), nao conteudo de mod: num pacote do jogo editado vem os da versao
# de quem fez o mod. O ttbmetadata.xml do PATCH de um mod sem ru-ru derruba a v1.24 no boot (sem memoria, eboot+0x4a8797).
BUILD_INFO = {'\\STUFF\\TTBMETADATA.XML'}
# Arquivos que o jogo le de um pacote so (o de maior prioridade, sem juntar os dos outros) e que tem o jogo inteiro: num
# pacote proprio (BARTVOICE) vem so as linhas do mod; o text.csv de 34 linhas no lugar do de 20 mil apaga os textos.
WHOLE_GAME_FILES = {'\\STUFF\\TEXT\\TEXT.CSV'}


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


def figure_dump(folder):
    """Gravacao de figura do Toy Pad (tag NFC NTAG213: .bin de 180 bytes, as vezes com 540 ou 572), nao mod."""
    bins = others = 0
    for root, _, files in os.walk(folder):
        for f in files:
            ext = os.path.splitext(f)[1].lower()
            if ext == '.bin' and os.path.getsize(os.path.join(root, f)) in (180, 540, 572):
                bins += 1
            elif ext not in ('.png', '.jpg', '.jpeg', '.txt', '.json', '.ini'):
                others += 1
    return bins > 0 and others == 0


def not_a_mod(label, folder):
    if figure_dump(folder):
        say('AVISO ' + T('%s: e a gravacao de uma figura (tag .bin do Toy Pad), nao um mod. Use no app do portal (LegoToypad) '
                         'e tire da pasta de mods' % label,
                         '%s: this is a figure dump (Toy Pad .bin tag), not a mod. Load it in the portal app (LegoToypad) '
                         'and remove it from the mods folder' % label))
    else:
        say('AVISO ' + T('%s: nada que o port saiba carregar (nem pacote .HDR/.DAT, nem pastas data\\ ou sce_sys\\)' % label,
                         '%s: nothing the port can load (no .HDR/.DAT pack, no data\\ or sce_sys\\ folders)' % label))


def find_packs(folder):
    """Pares .HDR + .DAT do mod: pacote proprio (AllWorld) ou um pacote do jogo inteiro editado (PATCH.DAT dos mods de shadPS4)."""
    packs = []
    base = folder.rstrip('\\/').count(os.sep)
    for root, dirs, files in os.walk(folder):
        if root.count(os.sep) - base >= 4:
            dirs[:] = []
        lower = {f.lower(): f for f in files}
        for f in files:
            stem, ext = os.path.splitext(f)
            if ext.lower() == '.hdr' and (stem + '.dat').lower() in lower:
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


def add_files_cc4(block, items):
    """Poe arquivos novos numa tabela .CC4 (PATCH): entradas e hashes na ordem dos hashes, e o indice +6 dos nos da
    arvore remapeado. Os nomes novos nao entram na arvore: o jogo acha um arquivo pelo hash FNV-1a do caminho.
    items: (nome, offset, tamanho guardado, tamanho real, comprimido). Devolve (tabela nova, nomes postos)."""
    archive_id, version, file_count, name_count, names_size = struct.unpack_from('>iiIII', block, 12)
    if version < 2 or archive_id > -10:
        raise ValueError('formato de tabela desconhecido')
    tree = 36 + names_size
    q = tree + 12 * name_count
    if struct.unpack_from('>iI', block, q) != (archive_id, file_count):
        raise ValueError('tabela fora do esperado')
    entries_at = q + 8
    hashes_at = entries_at + 16 * file_count
    rest = bytes(block[hashes_at + 4 * file_count:])
    old = [(struct.unpack_from('>I', block, hashes_at + 4 * i)[0], bytes(block[entries_at + 16 * i:entries_at + 16 * i + 16]), i)
           for i in range(file_count)]
    # confere a variante do hash com nomes que ja estao na tabela
    positions = toc_positions(bytes(block))[0]
    checked = 0
    for name, pos in list(positions.items())[:200]:
        index = (pos - entries_at) // 16
        if old[index][0] and old[index][0] != nu2toc.fnv(name[1:], 32, 1677619):
            raise ValueError('hash da tabela diferente do esperado')
        checked += 1
    if not checked:
        raise ValueError('tabela sem nomes conferiveis')
    taken = {h for h, _, _ in old}
    new, added = [], []
    for name, offset, zsize, usize, packed in items:
        h = nu2toc.fnv(name[1:], 32, 1677619)
        if not h or h in taken:
            continue
        taken.add(h)
        if archive_id <= -13:
            entry = struct.pack('>QII', offset | ((2 << 56) if packed else 0), zsize, usize)
        else:
            entry = struct.pack('>QII', offset, zsize, usize | (0x80000000 if packed else 0))
        new.append((h, entry, None))
        added.append(name)
    rows = sorted(old + new, key=lambda r: r[0])  # estavel: os de hash 0 (nomes por extenso) continuam no comeco
    remap = {r[2]: i for i, r in enumerate(rows) if r[2] is not None}
    out = bytearray(block[:entries_at])
    for i in range(name_count):  # so os nos de arquivo (+10 = numero do arquivo; o ultimo no e arquivo sempre)
        at = tree + 12 * i
        if struct.unpack_from('>I', out, at)[0] != 0xFFFFFFFF and (struct.unpack_from('>H', out, at + 10)[0] or i == name_count - 1):
            struct.pack_into('>H', out, at + 6, remap[struct.unpack_from('>H', out, at + 6)[0]])
    if len(rows) > 0xFFFF:
        raise ValueError('arquivos demais para o indice de 16 bits')
    struct.pack_into('>I', out, q + 4, len(rows))
    struct.pack_into('>I', out, 20, len(rows))
    out += b''.join(r[1] for r in rows)
    out += b''.join(struct.pack('>I', r[0]) for r in rows)
    out += rest
    struct.pack_into('>I', out, 0, len(out) - 4)
    return out, added


def csv_row(line):
    import csv
    return next(csv.reader([line]), [])


def merge_text(base, extras):
    """text.csv do jogo + as linhas (rotulos novos) do text.csv de cada mod; o jogo so le um text.csv (o do pacote de
    maior prioridade), entao as legendas e nomes de um mod de pacote proprio entram neste."""
    import csv
    import io
    text = base.decode('cp1252', errors='replace')
    newline = '\r\n' if '\r\n' in text else '\n'
    lines = text.split(newline)
    header = csv_row(lines[0])
    seen = {csv_row(l)[0].upper() for l in lines[1:] if l.strip() and csv_row(l)}
    added = []
    for label, data in extras:
        try:
            mod_text = data.decode('utf-8')
        except UnicodeDecodeError:
            mod_text = data.decode('cp1252', errors='replace')
        rows = list(csv.reader(io.StringIO(mod_text)))
        if not rows:
            continue
        mod_header = [h.upper() for h in rows[0]]
        for row in rows[1:]:
            if not row or not row[0] or row[0].upper() in seen:
                continue
            fields = dict(zip(mod_header, row))
            english = fields.get('ENGLISH') or next((v for v in row[3:] if v), '')
            out = io.StringIO()
            csv.writer(out, quoting=csv.QUOTE_ALL, lineterminator='').writerow(
                [fields.get(h.upper(), english if i >= 3 else '') for i, h in enumerate(header)])
            added.append(out.getvalue())
            seen.add(row[0].upper())
    if not added:
        return None, 0
    while lines and not lines[-1].strip():
        lines.pop()
    merged = newline.join(lines + added) + newline
    return merged.encode('cp1252', errors='replace'), len(added)


def read_ignore(folder):
    """mods\\<mod>\\ignorar.txt: um caminho interno por linha (ex. collectables\\cheats.txt) que o port nao usa do mod."""
    path = os.path.join(folder, 'ignorar.txt')
    names = set()
    if os.path.isfile(path):
        with open(path, encoding='utf-8-sig', errors='replace') as f:
            for line in f:
                line = line.split('#')[0].strip().replace('/', '\\')
                if line:
                    names.add(('\\' + line.lstrip('\\')).upper())
    return names


def loose_files(folder, top_dirs):
    """Arquivos soltos com o caminho interno do jogo, como os mods de PC (chars\\collection.txt na pasta do mod): so os
    que estao numa pasta de primeiro nivel que o jogo tem (chars, audio, stuff...), para deixar de fora leia-me e fotos."""
    out = []
    for base, dirs, files in os.walk(folder):
        for f in files:
            if os.path.splitext(f)[1].lower() in ('.hdr', '.dat', '.hdr2', '.dat2'):
                continue
            path = os.path.join(base, f)
            rel = os.path.relpath(path, folder).replace('/', '\\')
            if '\\' in rel and rel.split('\\')[0].lower() in top_dirs:
                out.append(('\\' + rel, path))
    return out


# ---------------------------------------------------------------- principal
def prepare(root, app0, patch, mods_dir, cache, addcont):
    os.makedirs(cache, exist_ok=True)
    manifest = os.path.join(cache, 'mods.txt')
    sources = mod_sources(mods_dir, app0)
    overlays, mod_files, loose_sources = [], [], []  # mod_files: (ordem da fonte, rotulo, .DAT, pasta)
    stamp = hashlib.sha1()
    with open(os.path.abspath(__file__), 'rb') as f:  # um mods.py novo (atualizacao do port) refaz o cache
        stamp.update(f.read())

    def stamp_file(path):
        st = os.stat(path)
        stamp.update(('%s|%d|%d' % (path, st.st_size, st.st_mtime_ns)).encode('utf-8', 'replace'))
    for source, (label, folder) in enumerate(sources):
        over = overlay_root(folder)
        if over:
            overlays.append(over)
            say('SOBREPOSICAO %s' % over)
            for base, _, files in os.walk(over):
                for f in files:
                    stamp_file(os.path.join(base, f))
        packs_here = find_packs(folder)
        for dat in packs_here:
            stem = os.path.splitext(os.path.basename(dat))[0]
            if over and GAME_PACK.match(stem) and os.path.normcase(dat).startswith(os.path.normcase(over) + os.sep):
                continue  # pacote do jogo dentro de data\chunkN: a sobreposicao ja troca o pacote inteiro
            stamp_file(dat)
            mod_files.append((source, label + ': ' + stem, dat, folder))
        if not over:
            loose_sources.append((source, label, folder))
            for base, _, files in os.walk(folder):
                for f in files:
                    if os.path.splitext(f)[1].lower() not in ('.hdr', '.dat', '.hdr2', '.dat2'):
                        stamp_file(os.path.join(base, f))
    layers = overlays + ([patch] if patch and os.path.isdir(patch) else []) + [app0]
    packs = game_packs(layers, addcont)
    for name, (hdr_path, dat_path) in sorted(packs.items()):
        stamp_file(hdr_path)
        stamp_file(dat_path)
    key = stamp.hexdigest()
    if os.path.exists(manifest):
        with open(manifest, encoding='utf-8') as f:
            if f.readline().strip() == '# chave ' + key:
                say(T('mods ja preparados (nada mudou)', 'mods already prepared (nothing changed)'))
                say('PRONTO %d' % len(sources))
                return 0
    for old in glob.glob(os.path.join(cache, '*.HDR')) + glob.glob(os.path.join(cache, '*.tail')):
        os.remove(old)

    # tabelas de todos os pacotes do jogo (nome -> posicao da entrada), para saber o que ja existe
    tocs = {}
    for pack, (hdr_path, dat_path) in sorted(packs.items()):
        with open(hdr_path, 'rb') as f:
            block = bytearray(f.read())
        try:
            tocs[pack] = (block,) + (toc_positions if block[4:8] == b'.CC4' else toc_positions_old)(bytes(block))
        except Exception as e:
            say('AVISO ' + T('tabela de %s nao lida (%s)' % (pack, e), 'table of %s not read (%s)' % (pack, e)))
    all_names = set()
    for _block, positions, _id, _version in tocs.values():
        all_names.update(positions)
    top_dirs = {n.split('\\')[1].lower() for n in all_names if n.count('\\') > 1}

    # arquivos de cada mod, por (escopo, nome): escopo None = pacote proprio (AllWorld) ou arquivo solto, que troca o
    # arquivo em todo pacote do jogo que o tem (ou entra como arquivo novo no PATCH); escopo PACOTE = pacote do jogo
    # editado, que so troca dentro do pacote de mesmo nome, como no shadPS4 (cada pacote tem o seu
    # stuff\ttbmetadata.xml: o do PATCH no GAME derruba o jogo). O primeiro mod (ordem alfabetica) que trouxer um
    # nome vence. Os text.csv de pacote proprio / soltos sao juntados ao do jogo (texts), nao trocados.
    wanted, texts = {}, []
    ignored = {}

    def take(scope, order, label, name, data, usize, packed, folder):
        upper = name.upper()
        if upper in ignored.setdefault(folder, read_ignore(folder)):
            return
        if (scope, upper) in wanted:
            return
        if not scope and upper in WHOLE_GAME_FILES:
            texts.append((label, data) if not packed else None)
            return
        if scope and upper in BUILD_INFO:
            say('AVISO ' + T('%s: %s do mod ignorado (descreve a versao do jogo de quem fez o mod; trocar derruba o jogo)' % (label, name),
                             '%s: the mod\'s %s was skipped (it describes the mod author\'s game version; replacing it crashes the game)' % (label, name)))
            return
        wanted[(scope, upper)] = (order, label, name, data, usize, packed)
    for source, label, dat, folder in mod_files:
        try:
            rows = nu2toc.read_toc(dat)
        except Exception as e:
            say('AVISO ' + T('%s: tabela do pacote ilegivel (%s)' % (label, e), '%s: unreadable pack table (%s)' % (label, e)))
            continue
        # pacote do jogo inteiro editado (ex. PATCH.DAT de 1 GB com 8 arquivos mudados): so entra o que difere do pacote
        # de mesmo nome do jogo, comparando os bytes guardados
        stem = os.path.splitext(os.path.basename(dat))[0].upper()
        same_pack = packs.get(stem) if GAME_PACK.match(stem) else None
        original = {}
        if same_pack:
            try:
                original = {row[0].upper(): row for row in nu2toc.read_toc(same_pack[1])}
            except Exception:
                same_pack = None
        scope = stem if same_pack else None
        with open(dat, 'rb') as f, open(same_pack[1] if same_pack else dat, 'rb') as g:
            for name, usize, zsize, offset, packed in rows:
                f.seek(offset)
                data = f.read(zsize)
                old = original.get(name.upper())
                if old and old[1] == usize and old[2] == zsize:
                    g.seek(old[3])
                    if g.read(zsize) == data:
                        continue
                if scope and name.upper() not in original:
                    scope_new = None  # arquivo que o pacote do jogo nao tem: entra como os de pacote proprio
                else:
                    scope_new = scope
                take(scope_new, (source, 0), label, name, data, usize, bool(packed), folder)
    for source, label, folder in loose_sources:
        for name, path in loose_files(folder, top_dirs):
            with open(path, 'rb') as f:
                data = f.read()
            take(None, (source, 1), label + T(' (soltos)', ' (loose)'), name, data, len(data), False, folder)
    texts = [t for t in texts if t]
    text_name = '\\stuff\\text\\text.csv'
    if texts and 'PATCH' in tocs and text_name.upper() in tocs['PATCH'][1]:
        current = wanted.get(('PATCH', text_name.upper()))
        if current:
            base = current[3]
        else:
            base = None
            for row in nu2toc.read_toc(packs['PATCH'][1]):
                if row[0].upper() == text_name.upper():
                    with open(packs['PATCH'][1], 'rb') as f:
                        f.seek(row[3])
                        base = f.read(row[2]) if not row[4] else None
        merged, count = merge_text(base, texts) if base else (None, 0)
        if merged:
            wanted[('PATCH', text_name.upper())] = ((-1, 0), T('textos dos mods', 'mod texts'), text_name, merged, len(merged), False)
            say('TEXTOS ' + T('%d linha(s) nova(s) dos mods no text.csv (%s)' % (count, ', '.join(l for l, _ in texts)),
                              '%d new line(s) from the mods in text.csv (%s)' % (count, ', '.join(l for l, _ in texts))))

    lines = ['# chave ' + key]
    lines += ['overlay\t%s' % o for o in overlays]
    found = set()
    new_names = {k for k in wanted if k[0] is None and k[1] not in all_names}
    for pack, (hdr_path, dat_path) in sorted(packs.items()):
        if pack not in tocs:
            continue
        block, positions, archive_id, version = tocs[pack]
        hits = {}  # nome -> chave de wanted (o mod de menor ordem entre os de escopo None e os deste pacote)
        for key_, entry in wanted.items():
            if key_[0] in (None, pack) and key_[1] in positions and (key_[1] not in hits or entry[0] < wanted[hits[key_[1]]][0]):
                hits[key_[1]] = key_
        extra = sorted(new_names) if pack == 'PATCH' and block[4:8] == b'.CC4' else []
        if not hits and not extra:
            continue
        base = os.path.getsize(dat_path)
        tail = bytearray()
        index_rows = []
        for n, key_ in hits.items():
            _order, label, name, data, usize, packed = wanted[key_]
            while len(tail) % 16:
                tail.append(0)
            write_entry(block, positions[n], archive_id, version, base + len(tail), len(data), usize, packed)
            index_rows.append((name, usize, len(data), len(tail), 2 if packed else 0))
            tail += data
            found.add(key_)
        if extra:
            items = []
            for key_ in extra:
                _order, label, name, data, usize, packed = wanted[key_]
                while len(tail) % 16:
                    tail.append(0)
                items.append((name, base + len(tail), len(data), usize, packed))
                index_rows.append((name, usize, len(data), len(tail), 2 if packed else 0))
                tail += data
            try:
                block, added = add_files_cc4(block, items)
                found.update((None, n.upper()) for n in added)
            except ValueError as e:
                say('AVISO ' + T('arquivos novos dos mods nao entraram no PATCH (%s)' % e, 'the mods\' new files were not added to PATCH (%s)' % e))
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
    per_mod = {}  # rotulo -> [trocados, novos, de fora]
    for label in [m[1] for m in mod_files]:
        per_mod[label] = [0, 0, 0]
    for key_, (_order, label, *_rest) in wanted.items():
        if _order[0] < 0:
            continue
        hit = per_mod.setdefault(label, [0, 0, 0])
        hit[(1 if key_ in new_names else 0) if key_ in found else 2] += 1
    for label, (ok, new, out) in per_mod.items():
        extra_text = T(', %d novo(s)' % new, ', %d new' % new) if new else ''
        say('MOD ' + T('%s: %d arquivo(s) do jogo trocado(s)%s' % (label, ok, extra_text), '%s: %d game file(s) replaced%s' % (label, ok, extra_text)))
        if out:
            say('AVISO ' + T('%s: %d arquivo(s) ficaram de fora (nome repetido ou o jogo nao tem o pacote)' % (label, out),
                             '%s: %d file(s) were left out (repeated name or the game lacks the pack)' % (label, out)))
    for source, label, folder in loose_sources:
        has_loose = any(k for k, v in wanted.items() if v[1].startswith(label + ' ('))
        if not has_loose and not any(m[0] == source for m in mod_files):
            not_a_mod(label, folder)
    with open(manifest, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')
    say('PRONTO %d' % len(sources))
    return 0


def unsafe(name, dest):
    """Caminho de dentro do arquivo que sairia da pasta do mod (absoluto, com unidade ou com ..)."""
    target = os.path.normpath(os.path.join(dest, name))
    return os.path.isabs(name) or ':' in name or not (target + os.sep).startswith(os.path.normpath(dest) + os.sep)


def run(cmd):
    import subprocess
    return subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', stdin=subprocess.DEVNULL,
                          creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))


def last_line(result):
    text = result.stderr.strip() or result.stdout.strip()
    return text.splitlines()[-1] if text else '?'


def extractors(archive):
    """(programa, listar, nomes da listagem, extrair em dest), na ordem de tentativa: o 7-Zip que vem com o port
    (tools\\7zip), o 7-Zip / WinRAR instalados e o tar.exe do Windows (libarchive: nao abre RAR v6 / dicionario grande)."""
    import shutil
    sevens, unrars = [os.path.join(HERE, '..', '7zip', '7z.exe')], []
    try:
        import winreg
        for hive in (winreg.HKEY_LOCAL_MACHINE, winreg.HKEY_CURRENT_USER):
            for key, value, out, exe in ((r'SOFTWARE\7-Zip', 'Path64', sevens, '7z.exe'), (r'SOFTWARE\7-Zip', 'Path', sevens, '7z.exe'),
                                         (r'SOFTWARE\WinRAR', 'exe64', unrars, None), (r'SOFTWARE\WinRAR', 'exe32', unrars, None)):
                try:
                    with winreg.OpenKey(hive, key) as k:
                        path = winreg.QueryValueEx(k, value)[0]
                    out.append(os.path.join(path, exe) if exe else os.path.join(os.path.dirname(path), 'UnRAR.exe'))
                except OSError:
                    pass
    except ImportError:
        pass
    for pf in (os.environ.get('ProgramFiles'), os.environ.get('ProgramFiles(x86)')):
        if pf:
            sevens.append(os.path.join(pf, '7-Zip', '7z.exe'))
            unrars.append(os.path.join(pf, 'WinRAR', 'UnRAR.exe'))
    sevens.append(shutil.which('7z') or '')
    unrars.append(shutil.which('unrar') or '')
    seen, found = set(), []

    def add(exe, *rest):
        if exe and os.path.isfile(exe) and os.path.normcase(os.path.abspath(exe)) not in seen:
            seen.add(os.path.normcase(os.path.abspath(exe)))
            found.append((exe,) + rest)
    for exe in sevens:
        add(exe, [exe, 'l', '-ba', '-slt', '-sccUTF-8', archive],
            lambda out: [line[7:] for line in out.splitlines() if line.startswith('Path = ')],
            lambda dest, exe=exe: [exe, 'x', '-y', '-bso0', '-bsp0', '-o' + dest, archive])
    for exe in unrars:
        add(exe, [exe, 'lb', '-p-', archive], lambda out: [line for line in out.splitlines() if line],
            lambda dest, exe=exe: [exe, 'x', '-y', '-p-', '-idq', archive, dest + os.sep])
    tar = os.path.join(os.environ.get('SystemRoot', r'C:\Windows'), 'System32', 'tar.exe')
    add(tar, [tar, '-tf', archive], lambda out: [line for line in out.splitlines() if line],
        lambda dest: [tar, '-xf', archive, '-C', dest])
    return found


def install(archive, mods_dir):
    """Extrai um mod (.zip, .rar ou .7z) em mods\\<nome do arquivo>; recusa caminhos que sairiam da pasta.
    .zip pelo zipfile; o resto (e .zip que o zipfile nao abre) pelos programas de extractors()."""
    import shutil
    import zipfile
    base = os.path.basename(archive)
    name = re.sub(r'[<>:"/\\|?*]', '_', os.path.splitext(base)[0]).strip(' .') or 'mod'
    dest, n = os.path.join(mods_dir, name), 2
    while os.path.exists(dest):
        dest = os.path.join(mods_dir, '%s (%d)' % (name, n))
        n += 1

    def refuse(bad):
        say('ERRO ' + T('o arquivo tem um caminho perigoso (%s): nada foi instalado' % bad,
                        'the archive has an unsafe path (%s): nothing was installed' % bad))
        return 1
    done, errors = False, []
    if zipfile.is_zipfile(archive):
        try:
            with zipfile.ZipFile(archive) as z:
                bad = next((i.filename for i in z.infolist() if unsafe(i.filename, dest)), None)
                if bad:
                    return refuse(bad)
                os.makedirs(dest)
                z.extractall(dest)
            done = True
        except (NotImplementedError, RuntimeError, zipfile.BadZipFile) as e:  # metodo que o zipfile nao tem, ou senha
            shutil.rmtree(dest, ignore_errors=True)
            errors.append('zipfile: %s' % e)
    for exe, list_cmd, names, extract in ([] if done else extractors(archive)):
        tool = os.path.basename(exe)
        listing = run(list_cmd)
        found = names(listing.stdout) if listing.returncode == 0 else []
        if not found:
            errors.append('%s: %s' % (tool, last_line(listing)))
            continue
        bad = next((f for f in found if unsafe(f, dest)), None)
        if bad:
            return refuse(bad)
        os.makedirs(dest)
        result = run(extract(dest))
        if result.returncode == 0:
            done = True
            break
        shutil.rmtree(dest, ignore_errors=True)
        errors.append('%s: %s' % (tool, last_line(result)))
    if not done:
        say('ERRO ' + T('nao consegui abrir %s (%s). Extraia com o 7-Zip / WinRAR e ponha a pasta em mods\\' % (base, '; '.join(errors)),
                        'could not open %s (%s). Extract it with 7-Zip / WinRAR and put the folder in mods\\' % (base, '; '.join(errors))))
        return 1
    if not overlay_root(dest) and figure_dump(dest):
        shutil.rmtree(dest, ignore_errors=True)
        say('ERRO ' + T('%s e a gravacao de uma figura (tag .bin do Toy Pad), nao um mod: nada foi instalado. Abra o .bin no app '
                        'do portal (LegoToypad)' % base,
                        '%s is a figure dump (Toy Pad .bin tag), not a mod: nothing was installed. Open the .bin in the portal '
                        'app (LegoToypad)' % base))
        return 1
    if not overlay_root(dest) and not find_packs(dest):
        say('AVISO ' + T('%s: nao tem pacote .HDR/.DAT nem pastas data\\ ou sce_sys\\; confira se e um mod de PS4' % os.path.basename(dest),
                         '%s: no .HDR/.DAT pack and no data\\ or sce_sys\\ folders; check that it is a PS4 mod' % os.path.basename(dest)))
    say('INSTALADO ' + os.path.basename(dest))
    return 0


def main():
    global LANG
    parser = argparse.ArgumentParser(description='LEGO Dimensions PC: mods')
    parser.add_argument('--instalar', help='extrai um mod (.zip, .rar ou .7z) na pasta de mods')
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
