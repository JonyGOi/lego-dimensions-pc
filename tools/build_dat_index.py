#!/usr/bin/env python3
"""Gera o indice (nome -> offset/tamanho) de cada pacote .DAT do jogo, usado pelo "radar de telas".

Os arquivos do jogo ficam dentro dos .DAT e o eboot so abre os pacotes, entao o nome do que
esta sendo carregado nunca passa pelo sistema de arquivos: o radar precisa deste indice para
traduzir (pacote, offset lido) em nome de arquivo.

A leitura do formato e feita pelo ReflectDat (wrapper do DATManager), que so LE os .DAT.
Saida: tools/data/dat_index/<PACOTE>.tsv  com  nome <TAB> tamanho <TAB> empacotado <TAB> offset <TAB> metodo

Fase 47: tambem os pacotes dos DLCs (.DAT2 + .HDR2, um por pasta em --addcont).

uso: python tools/build_dat_index.py [--app0 DIR] [--addcont DIR] [--only-dlc] [--reflectdat EXE]
"""
import argparse
import os
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_APP0 = r"C:\Jogos\CUSA01176"
DEFAULT_ADDCONT = str(pathlib.Path(os.environ.get("APPDATA", "")) / "shadPS4" / "addcont" / "CUSA01176")
DEFAULT_TOOL = r"ReflectDat.exe"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--app0", default=DEFAULT_APP0)
    ap.add_argument("--reflectdat", default=DEFAULT_TOOL)
    ap.add_argument("--addcont", default=DEFAULT_ADDCONT, help="pasta dos DLCs (uma subpasta por DLC); vazio = sem DLC")
    ap.add_argument("--only-dlc", action="store_true", help="so os pacotes dos DLCs")
    args = ap.parse_args()

    out_dir = ROOT / "tools" / "data" / "dat_index"
    out_dir.mkdir(parents=True, exist_ok=True)
    dats = [] if args.only_dlc else sorted(pathlib.Path(args.app0, "data").glob("chunk*/*.DAT"))
    if args.addcont and pathlib.Path(args.addcont).is_dir():
        dats += sorted(pathlib.Path(args.addcont).glob("*/*.DAT2"))  # so leitura
    if not dats:
        sys.exit(f"nenhum .DAT em {args.app0}\data\chunk*")
    total = 0
    for dat in dats:
        out = out_dir / (dat.stem.upper() + ".tsv")
        r = subprocess.run([args.reflectdat, "list", str(dat), str(out)], capture_output=True, text=True)
        if r.returncode != 0 or not out.exists():
            print(f"FALHOU {dat.name}: {(r.stderr or r.stdout).strip()[:200]}")
            continue
        n = sum(1 for _ in open(out, encoding="utf-8", errors="replace"))
        total += n
        print(f"{dat.name:22s} {n:7d} entradas")
    print(f"total: {total} entradas em {len(dats)} pacotes -> {out_dir}")


if __name__ == "__main__":
    main()
