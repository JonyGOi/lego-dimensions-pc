#!/usr/bin/env python3
"""
Fase 2.1 - Resolucao de nomes dos NIDs importados pelo eboot.bin.

Fontes, em ordem:
  1. Banco local de NIDs (aerolib): tools/data/aerolib.csv, ou um aerolib.inl do shadPS4
     (--db CAMINHO). Na primeira execucao o .inl e convertido para o CSV local.
  2. Os proprios sys_modules/: quem exporta cada NID (endereco, tamanho) e se algum modulo
     traz nomes em texto claro (.symtab / section headers).
  3. Forca bruta: strings com cara de identificador extraidas do eboot e dos modulos,
     passadas pelo hash de NID (SHA1 + sufixo, 8 bytes LE, base64 com "+-").

Todo nome encontrado e verificado recalculando o NID. Saida: FASE2_nids.json
"""
from __future__ import annotations

import argparse
import base64
import csv
import hashlib
import json
import re
import struct
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check_binaries as cb  # noqa: E402

ROOT = cb.ROOT
DB_CSV = ROOT / "tools" / "data" / "aerolib.csv"
DEFAULT_INL = [
    Path.home() / "code" / "shadPS4" / "src" / "core" / "aerolib" / "aerolib.inl",
]
OUT_JSON = ROOT / "FASE2_nids.json"

NID_SUFFIX = bytes.fromhex("518D64A635DED8C1E6B039B1C3E55230")
STUB_RE = re.compile(r'STUB\(\s*"([^"]{11})",\s*([^\s)]+)\s*\)')  # entradas podem quebrar linha
IDENT_RE = re.compile(rb"[A-Za-z_][A-Za-z0-9_]{2,127}")


def calc_nid(name: str) -> str:
    digest = hashlib.sha1(name.encode() + NID_SUFFIX).digest()
    value = struct.unpack("<Q", digest[:8])[0]
    return base64.b64encode(struct.pack(">Q", value), altchars=b"+-").decode().rstrip("=")


def load_db(explicit: Path | None) -> tuple[dict[str, str], str]:
    """Retorna ({nid: nome}, descricao da origem)."""
    if explicit is None and DB_CSV.is_file():
        with DB_CSV.open(newline="", encoding="utf-8") as f:
            return {nid: name for nid, name in csv.reader(f)}, str(DB_CSV.relative_to(ROOT))
    for inl in ([explicit] if explicit else DEFAULT_INL):
        if not inl.is_file():
            continue
        db = dict(STUB_RE.findall(inl.read_text(encoding="utf-8")))
        DB_CSV.parent.mkdir(parents=True, exist_ok=True)
        with DB_CSV.open("w", newline="", encoding="utf-8") as f:
            csv.writer(f).writerows(sorted(db.items()))
        return db, f"{inl} (convertido para {DB_CSV.relative_to(ROOT)})"
    return {}, "NENHUM banco local encontrado"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--db", type=Path, help="aerolib.inl alternativo")
    args = ap.parse_args()
    sys.stdout.reconfigure(encoding="utf-8")

    fase1 = json.loads(cb.IMPORTS_JSON.read_text(encoding="utf-8"))
    imports = fase1["imports"]
    wanted = {i["nid"] for i in imports}

    db, db_src = load_db(args.db)
    print(f"Banco de NIDs : {db_src} ({len(db)} entradas)")

    # ---- sys_modules: exports e nomes em texto claro ----------------------
    sys_modules = cb.find_sys_modules()
    exports: dict[tuple[str, str], dict] = {}  # (nid, lib) -> info
    exports_by_nid: dict[str, list[dict]] = {}
    with_sections, with_symtab, plain_names, total_exports = [], [], 0, 0
    blobs: dict[str, bytes] = {}
    needed_files = {n["file"] for n in fase1["needed"] if n["file"]}
    for path in sorted(sys_modules.iterdir()) if sys_modules else []:
        if not path.is_file():
            continue
        elf, _kind, _s = cb.load_elf(path)
        if elf is None:
            continue
        e_shoff, e_shnum = struct.unpack_from("<Q", elf, 0x28)[0], struct.unpack_from("<H", elf, 0x3C)[0]
        if e_shnum and e_shoff and e_shoff < len(elf):
            with_sections.append(path.name)
            for i in range(e_shnum):
                sh_type = struct.unpack_from("<I", elf, e_shoff + i * 0x40 + 4)[0]
                if sh_type == 2:  # SHT_SYMTAB
                    with_symtab.append(path.name)
                    break
        info = cb.analyze_elf(elf)
        total_exports += len(info.exports)
        for nid, lib, value, size, kind in info.exports:
            if len(nid) != 11:
                plain_names += 1
            rec = {"file": path.name, "library": lib, "vaddr": value, "size": size, "type": kind}
            # varios arquivos exportam o mesmo NID (libkernel/_web/_sys, GnmDriver/ForNeoMode):
            # fica o modulo que o eboot realmente pede em DT_NEEDED
            if (nid, lib) not in exports or path.name in needed_files:
                exports[(nid, lib)] = rec
            exports_by_nid.setdefault(nid, []).append(rec)
        if path.name in needed_files:
            blobs[path.name] = elf

    # ---- resolucao --------------------------------------------------------
    names: dict[str, tuple[str, str]] = {}  # nid -> (nome, fonte)
    bad_db = 0
    for nid in wanted:
        name = db.get(nid)
        if name is None:
            continue
        if calc_nid(name) == nid:
            names[nid] = (name, "aerolib")
        else:
            bad_db += 1

    unresolved = wanted - names.keys()
    brute_candidates = 0
    if unresolved:
        blobs["eboot.bin"] = cb.load_elf(cb.ASSETS / "eboot.bin")[0]
        seen: set[bytes] = set()
        for fname, blob in blobs.items():
            for tok in set(IDENT_RE.findall(blob)) - seen:
                seen.add(tok)
                nid = calc_nid(tok.decode())
                if nid in unresolved:
                    names[nid] = (tok.decode(), f"forca bruta (string em {fname})")
        brute_candidates = len(seen)
        unresolved = wanted - names.keys()

    # ---- saida ------------------------------------------------------------
    out = []
    not_exported = []
    for imp in imports:
        nid = imp["nid"]
        name, src = names.get(nid, (None, None))
        exp = exports.get((nid, imp["library"]))
        if exp is None:
            # mesmo NID exportado por outra biblioteca do mesmo modulo (ex.: libScePosix em libkernel)
            cands = exports_by_nid.get(nid, [])
            cands = [c for c in cands if c["file"] in needed_files] or cands
            exp = cands[0] if len(cands) == 1 else None
        if exp is None:
            not_exported.append(imp)
        out.append({**imp, "name": name, "source": src,
                    "exported_by": exp["file"] if exp else None,
                    "export_vaddr": exp["vaddr"] if exp else None,
                    "size": exp["size"] if exp else None})

    by_src = Counter(o["source"] or "nao resolvido" for o in out)
    per_lib = {}
    for o in out:
        t = per_lib.setdefault(o["library"], [0, 0])
        t[0] += 1
        t[1] += o["name"] is not None
    stats = {
        "db_source": db_src,
        "db_entries": len(db),
        "db_hash_mismatch": bad_db,
        "total": len(out),
        "resolved": sum(o["name"] is not None for o in out),
        "by_source": dict(by_src),
        "brute_force_candidates": brute_candidates,
        "per_library": {k: {"total": v[0], "resolved": v[1]} for k, v in sorted(per_lib.items())},
        "sys_modules": {
            "files": len([p for p in sys_modules.iterdir() if p.is_file()]) if sys_modules else 0,
            "with_section_headers": with_sections,
            "with_symtab": with_symtab,
            "total_exports": total_exports,
            "exports_with_plaintext_name": plain_names,
            "imports_not_exported_by_any_module": [f'{i["nid"]} ({i["library"]})' for i in not_exported],
        },
    }
    OUT_JSON.write_text(json.dumps({"stats": stats, "imports": out}, indent=1), encoding="utf-8")

    print(f"NIDs importados          : {len(out)}")
    print(f"Resolvidos               : {stats['resolved']}  {dict(by_src)}")
    print(f"Hash divergente no banco : {bad_db}")
    if brute_candidates:
        print(f"Forca bruta              : {brute_candidates} strings testadas")
    for o in out:
        if o["name"] is None:
            print(f"   SEM NOME: {o['nid']}  {o['library']}  ({o['type']})")
    print(f"sys_modules              : {total_exports} exports, {plain_names} com nome em texto claro, "
          f"{len(with_sections)} arquivos com section headers, {len(with_symtab)} com .symtab")
    print(f"Imports sem exportador   : {len(not_exported)}")
    for i in not_exported:
        print(f"   {i['nid']}  {i['library']}  {names.get(i['nid'], ('?',))[0]}")
    print(f"Saida                    : {OUT_JSON}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
