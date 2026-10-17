#!/usr/bin/env python3
"""Monta uma imagem da INTERFACE de um dump de frame (texto, icones), com transparencia.

Nao e um renderizador completo: desenha os draws 2D como um retangulo por letra / sprite. O
layout do vertice de cada draw vem das linhas "layout:" do dump (stride, offset e formato de
posicao, UV e cor, tirados do fetch shader e dos descritores V# do draw). Dumps antigos, sem
essas linhas, caem no layout fixo da interface (36 bytes: posicao float3 em +0, cor RGBA8 em
+16, UV float2 em +20). Para cada draw usa o que o dump exportou:

  - layout: arquivo, stride, offset e formato de cada atributo; indices (DRAW_INDEX_2) quando ha;

  - matriz de mundo: primeiro buffer de constantes de 4 elementos da tabela propria do draw
    (origem "tabN.0"; as demais entradas das tabelas sao de draws vizinhos);
  - projecao: as 4 primeiras linhas do buffer de constantes embutido (origem "reg8");
  - cor do vertice (tinta e alfa) multiplicando a textura;
  - blend: LIGADO com SRC_ALPHA / ONE_MINUS_SRC_ALPHA = alfa normal; destino ONE = aditivo;
    desligado = copia opaca; mascara de cor 0 = draw ignorado.

Limites: retangulos alinhados aos eixos (rotacao vira caixa envolvente), sem scissor, sem
teste de profundidade, sem constantes do pixel shader (fades feitos no shader nao aparecem), e
o cenario 3D nao e desenhado.

uso: python tools/decode_frame_textures.py <pasta> BC
     python tools/render_frame_ui.py <pasta> <frame_NNN_draws_tXXXs.txt> [--no-matrix] [--all]
Requer Pillow.
"""
import pathlib
import re
import struct
import sys

from PIL import Image, ImageChops

W, H = 1920, 1080
DRAW = re.compile(r"^draw\s+(\d+)\s+(\S+)\s+(\d+).*prim=(\d+).*alvo 0x([0-9a-f]+)")
TEX = re.compile(r"^\s+(\d+)x(\d+)\s+(\S+)\s+numfmt \d+ tipo 0x\w+ niveis (\d+) tiling (\d+) pitch (\d+) @0x([0-9a-f]+)")
VB = re.compile(r"vertices: (geo_\S+_stride(\d+)\.bin)\s+(\d+) elementos")
LAYOUT = re.compile(r"layout: sem=(\d+) arquivo=(\S+) stride=(\d+) offset=(\d+) formato=(\S+) carregados=(\d+) papel=(\S+)")
CONST = re.compile(r"constantes VS: (geo_\S+\.bin)\s+(\d+) elementos @0x[0-9a-f]+\s+origem (\S+)")
BLEND = re.compile(r"blend: (\S+) cor src=(\S+) dst=(\S+) func=(\S+) \| alfa src=(\S+) dst=(\S+) func=(\S+) \|.*mascara=0x([0-9a-f]+)")


def parse(frame_path):
    draws, current = [], None
    for line in frame_path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = DRAW.match(line)
        if m:
            current = {"index": int(m[1]), "op": m[2], "count": int(m[3]), "prim": int(m[4]), "target": m[5],
                       "tex": [], "vb": [], "const": [], "blend": None, "layout": {}, "geo": None}
            draws.append(current)
            continue
        if line.startswith("===="):
            current = None
        if current is None:
            continue
        m = TEX.match(line)
        if m:
            current["tex"].append({"w": int(m[1]), "h": int(m[2]), "fmt": m[3], "levels": int(m[4]), "tiling": int(m[5]),
                                   "pitch": int(m[6]), "addr": m[7]})
        m = VB.search(line)
        if m:
            current["vb"].append((m[1], int(m[3]), int(m[2])))
        if "geometria: " in line:
            current["geo"] = line.split("geometria: ", 1)[1].split("  ", 1)[0].strip()
        m = LAYOUT.search(line)
        if m and m[2] != "-" and m[7] in ("posicao", "uv", "cor") and m[7] not in current["layout"]:
            current["layout"][m[7]] = {"file": m[2], "stride": int(m[3]), "offset": int(m[4]), "fmt": m[5]}
        m = CONST.search(line)
        if m:
            current["const"].append({"file": m[1], "count": int(m[2]), "origin": m[3]})
        m = BLEND.search(line)
        if m:
            current["blend"] = {"on": m[1] == "LIGADO", "src": m[2], "dst": m[3], "func": m[4], "mask": int(m[8], 16)}
    return draws


def half(h):
    return struct.unpack("<e", struct.pack("<H", h))[0]


def read_attribute(data, offset, fmt, want):
    """Le `want` componentes de um atributo no formato do dump (ex.: float32x3, float16x4,
    unorm8x4, snorm16x2). Devolve None se o formato nao for conhecido ou faltar dado."""
    m = re.fullmatch(r"(float|unorm|snorm|uscaled|sscaled|uint|sint|snorm_nz)(8|16|32)x(\d)", fmt)
    if not m:
        return None
    kind, bits, count = m[1], int(m[2]), int(m[3])
    size = bits // 8
    if offset + size * count > len(data):
        return None
    out = []
    for i in range(count):
        at = offset + i * size
        if kind == "float":
            out.append(struct.unpack_from("<f", data, at)[0] if bits == 32 else half(struct.unpack_from("<H", data, at)[0]))
            continue
        signed = kind in ("snorm", "sscaled", "sint", "snorm_nz")
        code = {8: "b", 16: "h", 32: "i"}[bits] if signed else {8: "B", 16: "H", 32: "I"}[bits]
        raw = struct.unpack_from("<" + code, data, at)[0]
        if kind == "unorm":
            raw = raw / float((1 << bits) - 1)
        elif kind in ("snorm", "snorm_nz"):
            raw = max(-1.0, raw / float((1 << (bits - 1)) - 1))
        out.append(raw)
    while len(out) < want:
        out.append(0.0 if len(out) < 3 else 1.0)
    return out[:want]


class Vertices:
    """Acesso aos vertices de um draw pelo layout do dump (ou pelo layout fixo antigo)."""

    def __init__(self, folder, draw):
        self.ok = False
        self.cache = {}
        self.folder = folder
        self.count = 0
        layout = draw["layout"]
        if "posicao" in layout and "uv" in layout:
            self.pos, self.uv, self.color = layout["posicao"], layout["uv"], layout.get("cor")
        else:
            legacy = [v for v in draw["vb"] if v[2] == 36 and v[1] >= draw["count"]]
            if not legacy or layout:
                return
            exact = [v for v in legacy if v[1] == draw["count"]]
            name = (exact or legacy)[0][0]
            self.pos = {"file": name, "stride": 36, "offset": 0, "fmt": "float32x3"}
            self.color = {"file": name, "stride": 36, "offset": 16, "fmt": "unorm8x4"}
            self.uv = {"file": name, "stride": 36, "offset": 20, "fmt": "float32x2"}
        used = [self.pos, self.uv] + ([self.color] if self.color else [])
        self.ok = all(self._data(a) is not None for a in used)
        if self.ok:
            self.count = len(self._data(self.pos)) // self.pos["stride"]

    def _data(self, attribute):
        name = attribute["file"]
        if name not in self.cache:
            path = self.folder / name
            self.cache[name] = path.read_bytes() if path.exists() else None
        return self.cache[name]

    def get(self, index):
        """(x, y, z), (u, v), (r, g, b, a em 0..254) do vertice `index`, ou None."""
        p = read_attribute(self._data(self.pos), index * self.pos["stride"] + self.pos["offset"], self.pos["fmt"], 3)
        t = read_attribute(self._data(self.uv), index * self.uv["stride"] + self.uv["offset"], self.uv["fmt"], 2)
        if p is None or t is None:
            return None
        color = (254, 254, 254, 254)
        if self.color:
            c = read_attribute(self._data(self.color), index * self.color["stride"] + self.color["offset"], self.color["fmt"], 4)
            if c is not None:
                color = tuple(int(round(v * 254)) for v in c)  # 0xFE = 1.0 nesta engine
        return p, t, color


def indices_of(folder, frame_stem, draw):
    """Indices de um DRAW_INDEX_2 (geo_<frame>_draw<n>_idx<2|4>.bin), ou 0..n-1 para DRAW_INDEX_AUTO."""
    frame_draws = frame_stem.split("_")[1]
    for size, code in ((2, "H"), (4, "I")):
        path = folder / f"geo_{frame_draws}_draw{draw['index']:03d}_idx{size}.bin"
        if path.exists():
            data = path.read_bytes()
            return list(struct.unpack(f"<{len(data) // size}{code}", data))
    return list(range(draw["count"])) if draw["op"] == "DRAW_INDEX_AUTO" else None


def floats(folder, name, count):
    data = (folder / name).read_bytes()
    count = min(count, len(data) // 4)
    return struct.unpack(f"<{count}f", data[:count * 4])


def matrices(folder, draw, use_matrix):
    """(mundo, projecao) como listas de 16 floats em ordem de linha, ou None."""
    world = projection = None
    for const in draw["const"]:
        if const["origin"] == "reg8" and const["count"] >= 4:
            m = floats(folder, const["file"], 16)
            if len(m) == 16 and m[0] > 0.1 and m[5] > 0.1:
                projection = m
        if use_matrix and world is None and const["count"] == 4 and re.fullmatch(r"tab\d+\.0", const["origin"]):
            m = floats(folder, const["file"], 16)
            if len(m) == 16 and abs(m[15] - 1.0) < 1e-3 and all(abs(v) < 1e4 for v in m):
                world = m
    if projection is None and use_matrix:
        # Outro vertex shader (textos grandes de aviso): nao ha projecao em reg8 e a posicao,
        # depois da matriz de mundo, ja esta em coordenadas de tela normalizadas (-1..1).
        # A matriz e um buffer de 4 elementos com escala; a origem varia de draw para draw.
        affine = []
        for const in draw["const"]:
            if const["count"] != 4:
                continue
            m = floats(folder, const["file"], 16)
            if (len(m) == 16 and abs(m[15] - 1.0) < 1e-3 and m[3] == m[7] == m[11] == 0.0 and m[1] == m[4] == 0.0
                    and 0.01 < abs(m[0]) < 10 and 0.01 < abs(m[5]) < 10):
                affine.append(m)
        scaled = [m for m in affine if abs(m[0] - 1.0) > 1e-3 or abs(m[5] - 1.0) > 1e-3]
        world = (scaled or affine or [None])[0]
        return world, "ndc"
    return world, projection


def project(x, y, z, world, projection):
    if world:
        x, y, z = (x * world[0] + y * world[4] + z * world[8] + world[12],
                   x * world[1] + y * world[5] + z * world[9] + world[13],
                   x * world[2] + y * world[6] + z * world[10] + world[14])
    if projection == "ndc":
        return 960 * (1 + x), 540 * (1 - y)
    if projection:
        cx = x * projection[0] + y * projection[4] + z * projection[8] + projection[12]
        cy = x * projection[1] + y * projection[5] + z * projection[9] + projection[13]
        cw = x * projection[3] + y * projection[7] + z * projection[11] + projection[15]
        if cw <= 1e-6:
            return None
        return 960 * (1 + cx / cw), 540 * (1 - cy / cw)
    return 960 + x * 1080, 540 - y * 1080


def texture_png(folder, tex):
    name = f"tex_{tex['addr']}_{tex['w']}x{tex['h']}_p{tex['pitch']}_{tex['fmt']}_t{tex['tiling']}.png"
    path = folder / "png" / name
    return path if path.exists() else None


def blend_onto(canvas, layer, blend):
    if blend and blend["mask"] == 0:
        return
    if not blend or not blend["on"]:
        canvas.paste(layer, (0, 0), layer.split()[3].point(lambda a: 255 if a else 0))
    elif blend["dst"] == "ONE":
        rgb = Image.composite(layer, Image.new("RGBA", layer.size, (0, 0, 0, 0)), layer.split()[3]).convert("RGB")
        canvas.paste(ImageChops.add(canvas.convert("RGB"), rgb))
    else:  # SRC_ALPHA / ONE_MINUS_SRC_ALPHA e variantes
        canvas.alpha_composite(layer)


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    folder = pathlib.Path(sys.argv[1])
    frame = folder / sys.argv[2]
    use_matrix = "--no-matrix" not in sys.argv
    draw_all = "--all" in sys.argv
    draws = parse(frame)
    canvas = Image.new("RGBA", (W, H), (24, 24, 32, 255))
    cache = {}
    drawn = 0
    skipped = {}
    for draw in draws:
        if not draw["vb"] and not draw["layout"]:
            continue
        # So a interface: draws que o radar de geometria classificou como "UI 2D" (dumps
        # antigos, sem essa linha, passam). --all desenha tambem planos e geometria 3D, como
        # caixas envolventes (util para ver particulas e sprites do cenario).
        if not draw_all and draw["geo"] is not None and not draw["geo"].startswith("UI 2D"):
            skipped["nao e interface (classe: plano / 3D / tela cheia)"] = skipped.get("nao e interface (classe: plano / 3D / tela cheia)", 0) + 1
            continue
        # textura: prefere as de um nivel so (atlas de fonte / interface); senao a primeira 2D
        with_png = [t for t in draw["tex"] if t["fmt"] in ("BC1", "BC3", "RGBA8") and texture_png(folder, t)]
        one_level = [t for t in with_png if t["levels"] == 1]
        vertices = Vertices(folder, draw)
        indices = indices_of(folder, frame.stem, draw) if vertices.ok else None
        reason = ("sem textura decodificada (BC1 / BC3 / RGBA8)" if not with_png else "layout de vertice desconhecido" if not vertices.ok
                  else "sem indices" if indices is None else None)
        if reason:
            skipped[reason] = skipped.get(reason, 0) + 1
            continue
        # Qual textura o draw amostra nao esta no dump (dependeria de ler o shader). Ordem de
        # preferencia: BC de um nivel (atlas de fonte / interface), BC com mips, RGBA8 que nao
        # seja minuscula, e por ultimo as de 4x4 / 8x8 (cor solida). Neste jogo as RGBA8 sao
        # tabelas (gradiente, ruido, pontilhado), nao arte.
        def rank(t):
            tiny = t["w"] <= 8 or t["h"] <= 8
            return (tiny, t["fmt"] == "RGBA8", t["levels"] != 1)
        tex = sorted(with_png, key=rank)[0]
        path = texture_png(folder, tex)
        atlas = cache.setdefault(path, Image.open(path).convert("RGBA"))
        world, projection = matrices(folder, draw, use_matrix)
        per_quad = 6 if draw["prim"] == 4 else 4
        layer = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        quads = 0
        alpha_seen = set()
        for q in range(len(indices) // per_quad):
            xs, ys, us, vs = [], [], [], []
            color = (254, 254, 254, 254)
            for i in range(per_quad):
                index = indices[q * per_quad + i]
                vertex = vertices.get(index) if index < vertices.count else None
                if vertex is None:
                    continue
                (x, y, z), (u, v), color = vertex
                point = project(x, y, z, world, projection)
                if point is None:
                    continue
                xs.append(point[0]); ys.append(point[1]); us.append(u); vs.append(v)
            if len(xs) < 3:
                continue
            x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
            u0, u1 = min(us) * atlas.width, max(us) * atlas.width
            v0, v1 = min(vs) * atlas.height, max(vs) * atlas.height
            if x1 - x0 < 1 or y1 - y0 < 1 or u1 - u0 < 1 or v1 - v0 < 1 or x1 < 0 or y1 < 0 or x0 > W or y0 > H:
                continue
            if not (-1 <= u0 and u1 <= atlas.width + 1 and -1 <= v0 and v1 <= atlas.height + 1):
                continue
            if (x1 - x0) * (y1 - y0) > 0.6 * W * H or x1 - x0 > 4 * W or y1 - y0 > 4 * H:
                continue  # fundo de tela inteira (fade, moldura): esconderia o resto
            sprite = atlas.crop((int(u0), int(v0), int(round(u1)), int(round(v1))))
            sprite = sprite.resize((max(1, int(round(x1 - x0))), max(1, int(round(y1 - y0)))), Image.BILINEAR)
            scale = [max(0.0, min(1.0, c / 254.0)) for c in color]
            alpha_seen.add(color[3])
            if scale != [1.0, 1.0, 1.0, 1.0]:
                bands = [band.point(lambda p, s=s: int(p * s)) for band, s in zip(sprite.split(), scale)]
                sprite = Image.merge("RGBA", bands)
            piece = Image.new("RGBA", (W, H), (0, 0, 0, 0))
            piece.paste(sprite, (int(round(x0)), int(round(y0))))
            layer.alpha_composite(piece)
            quads += 1
        if quads:
            blend_onto(canvas, layer, draw["blend"])
            drawn += 1
            blend = draw["blend"]
            state = "sem estado" if not blend else ("desligado" if not blend["on"] else f"{blend['src']}/{blend['dst']}")
            print(f"draw {draw['index']:3d}: {quads:3d} ret. pos {vertices.pos['fmt']}+{vertices.pos['offset']} "
                  f"uv {vertices.uv['fmt']}+{vertices.uv['offset']} stride {vertices.pos['stride']} tex {tex['w']}x{tex['h']} "
                  f"blend {state} alfa {sorted(alpha_seen)} mundo={'sim' if world else 'nao'} "
                  f"proj={'ndc' if projection == 'ndc' else 'sim' if projection else 'nao'}")
        else:
            why = "nenhum retangulo visivel (fora da tela, UV fora da textura ou matriz desconhecida)"
            skipped[why] = skipped.get(why, 0) + 1
    for reason, count in sorted(skipped.items()):
        print(f"  {count:3d} draw(s) com vertices nao desenhados: {reason}")
    out = folder / (frame.stem + "_ui.png")
    canvas.convert("RGB").save(out)
    print(f"{drawn} draws de interface -> {out}")


if __name__ == "__main__":
    main()
