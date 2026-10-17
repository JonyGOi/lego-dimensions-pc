#!/usr/bin/env python3
"""Fase 49: preparacao do port a partir do dump do PROPRIO usuario (rodada pelo launcher).

Nada do jogo vem junto com o port: este script le o dump (pasta CUSA01176) e as DLCs de quem
instalou e gera, dentro da pasta do port:
  1. assets/decrypted_elfs/eboot.bin.elf  (o eboot.bin do dump e um FSELF sem criptografia: so tira o
     cabecalho, sem chave nenhuma)
  2. assets/shaders_aot/aot_pairs.pak/.idx (shaders do jogo e das DLCs traduzidos para Vulkan antes de
     jogar: extract + plan + compile do nu2_shader_extractor)

Saida para o launcher: linhas "PROGRESSO <0-100> <texto>" e "ERRO <texto>"; o resto e log.
uso: preparar.py --app0 PASTA_DO_JOGO [--addcont PASTA_DAS_DLCS] [--jobs N] [--sem-aot]
"""
import argparse
import os
import re
import struct
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
EXTRACTOR = os.path.join(ROOT, 'tools', 'nu2_shader_extractor', 'nu2_shader_extractor.py')
TITLE_ID = 'CUSA01176'


def progress(pct, text):
    print('PROGRESSO %d %s' % (pct, text), flush=True)


def fail(text):
    print('ERRO ' + text, flush=True)
    sys.exit(1)


def read_sfo(path):
    """Campos de um param.sfo (formato PSF)."""
    data = open(path, 'rb').read()
    if data[:4] != b'\0PSF':
        return {}
    keys, values, count = struct.unpack_from('<III', data, 8)
    out = {}
    for i in range(count):
        key_off, fmt, length, _, value_off = struct.unpack_from('<HHIII', data, 20 + 16 * i)
        key = data[keys + key_off:data.index(b'\0', keys + key_off)].decode('latin-1')
        raw = data[values + value_off:values + value_off + length]
        out[key] = struct.unpack('<I', raw[:4])[0] if fmt == 0x0404 else raw.rstrip(b'\0').decode('utf-8', 'replace')
    return out


def game_file(app0, patch, *parts):
    """Fase 58: o arquivo da atualizacao (pasta separada) vence o do jogo base."""
    if patch and os.path.isfile(os.path.join(patch, *parts)):
        return os.path.join(patch, *parts)
    return os.path.join(app0, *parts)


def check_game(app0, patch=''):
    sfo = game_file(app0, patch, 'sce_sys', 'param.sfo')
    if not os.path.isfile(game_file(app0, patch, 'eboot.bin')) or not os.path.isfile(sfo):
        fail('a pasta do jogo precisa ter eboot.bin e sce_sys\\param.sfo (dump do PS4 do LEGO Dimensions, CUSA01176)')
    info = read_sfo(sfo)
    if info.get('TITLE_ID') != TITLE_ID:
        fail('o dump e de outro jogo ou regiao (%s); este port e do LEGO Dimensions %s' % (info.get('TITLE_ID'), TITLE_ID))
    dats = set()
    for base in [app0] + ([patch] if patch else []):
        if os.path.isdir(os.path.join(base, 'data')):
            dats.update(f.upper() for d in os.listdir(os.path.join(base, 'data')) if d.lower().startswith('chunk')
                        for f in os.listdir(os.path.join(base, 'data', d)) if f.upper().endswith('.DAT'))
    if not dats:
        fail('nenhum pacote .DAT em data\\chunk*: o dump esta incompleto')
    print('jogo: %s %s, versao %s, %d pacotes .DAT%s' % (info.get('TITLE_ID'), info.get('TITLE', ''), info.get('APP_VER', '?'), len(dats),
                                                       (' (atualizacao em %s)' % patch) if patch else ''), flush=True)
    if not any(d.startswith('PATCH') for d in dats):
        print('AVISO: sem os pacotes PATCH*.DAT da atualizacao. O port foi feito para a versao 01.24: se a atualizacao estiver '
              'numa pasta separada, escolha-a em "Atualizacao" no launcher.', flush=True)
    elif info.get('APP_VER', '01.24') != '01.24':
        print('AVISO: versao %s; o port foi feito e testado com a 01.24' % info.get('APP_VER'), flush=True)
    return info


def check_dlcs(addcont):
    if not addcont:
        return 0
    if not os.path.isdir(addcont):
        print('AVISO: pasta de DLCs nao encontrada (%s): seguindo sem DLC' % addcont, flush=True)
        return 0
    n = 0
    for d in sorted(os.listdir(addcont)):
        sfo = os.path.join(addcont, d, 'sce_sys', 'param.sfo')
        if os.path.isfile(sfo):
            info = read_sfo(sfo)
            if str(info.get('CATEGORY', '')).startswith('ac'):
                n += 1
                print('DLC: %s (%s)' % (d, info.get('CONTENT_ID', '')), flush=True)
    print('%d DLC(s) encontradas' % n, flush=True)
    return n


def convert_eboot(app0, patch=''):
    import check_binaries
    out_dir = os.path.join(ROOT, 'assets', 'decrypted_elfs')
    os.makedirs(out_dir, exist_ok=True)
    data = open(game_file(app0, patch, 'eboot.bin'), 'rb').read()  # so leitura (Fase 58: o da atualizacao, se separada)
    if data[:4] == b'\x7fELF':
        elf = data
    else:
        elf, info = check_binaries.self_to_elf(data)
        if elf is None:
            fail('eboot.bin criptografado (SELF de varejo): use um dump descriptografado do jogo. ' + '; '.join(info.notes))
    with open(os.path.join(out_dir, 'eboot.bin.elf'), 'wb') as f:
        f.write(elf)
    print('eboot.bin.elf: %d bytes' % len(elf), flush=True)


def run_step(args, label, start, end):
    """Roda uma etapa do extrator; 'lote N/M' vira progresso entre start e end."""
    proc = subprocess.Popen([sys.executable, '-u', EXTRACTOR] + args, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, encoding='utf-8', errors='replace')
    for line in proc.stdout:
        line = line.rstrip()
        print(line, flush=True)
        m = re.search(r'lote (\d+)/(\d+)', line)
        if m and int(m.group(2)):
            progress(start + (end - start) * int(m.group(1)) // int(m.group(2)), '%s (%s/%s)' % (label, m.group(1), m.group(2)))
        m = re.search(r'^\s+(\d+) arquivos, ', line)
        if m:
            progress(start + min(end - start - 1, (end - start) * int(m.group(1)) // 14200), label)
    if proc.wait() != 0:
        fail('%s falhou (codigo %d)' % (label, proc.returncode))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--app0', required=True)
    ap.add_argument('--addcont', default='')
    ap.add_argument('--patch', default='', help='pasta da atualizacao separada (padrao: procura CUSA01176-patch)')
    ap.add_argument('--jobs', type=int, default=max(1, (os.cpu_count() or 4) - 2))
    ap.add_argument('--sem-aot', action='store_true', help='so o eboot (os shaders ficam para o compilador ao vivo)')
    ap.add_argument('--limite', type=int, default=0, help='so os N primeiros lotes da traducao (teste)')
    args = ap.parse_args()
    t0 = time.time()
    progress(1, 'conferindo o jogo')
    if not args.patch:  # Fase 58
        sys.path.insert(0, os.path.join(ROOT, 'tools', 'nu2_shader_extractor'))
        import nu2dat
        args.patch = nu2dat.find_patch(args.app0)
    check_game(args.app0, args.patch)
    check_dlcs(args.addcont)
    progress(3, 'convertendo o eboot.bin')
    convert_eboot(args.app0, args.patch)
    for d in ('shaders_spv', 'shaders_aot', 'dumped_shaders', 'savedata0', 'temp0', 'download0'):
        os.makedirs(os.path.join(ROOT, 'assets', d), exist_ok=True)
    if not args.sem_aot:
        addcont = ['--addcont', args.addcont] if args.addcont and os.path.isdir(args.addcont) else ['--addcont', '']
        progress(5, 'extraindo os shaders dos pacotes do jogo e das DLCs')
        run_step(['extract', '--app0', args.app0] + addcont + (['--patch', args.patch] if args.patch else []), 'extraindo shaders', 5, 15)
        progress(15, 'planejando as variantes')
        run_step(['plan'], 'planejando', 15, 17)
        progress(17, 'traduzindo os shaders para Vulkan (a etapa mais longa)')
        run_step(['compile', '--jobs', str(args.jobs)] + (['--limit', str(args.limite)] if args.limite else []), 'traduzindo shaders', 17, 99)
    with open(os.path.join(ROOT, 'assets', 'preparado.txt'), 'w', encoding='utf-8') as f:
        f.write('app0=%s\npatch=%s\naddcont=%s\naot=%s\n' % (args.app0, args.patch, args.addcont, 'nao' if args.sem_aot else 'sim'))
    progress(100, 'pronto em %.0f min' % ((time.time() - t0) / 60))


if __name__ == '__main__':
    main()
