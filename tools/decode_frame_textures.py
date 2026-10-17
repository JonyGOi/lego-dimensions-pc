#!/usr/bin/env python3
"""Converte as texturas brutas de um dump de frame (tex_*.bin) em PNG com transparencia.

Os arquivos sao o nivel 0 como esta na memoria da GPU do PS4. Formatos: BC1, BC3 e RGBA8.
O modo de tiling vem no nome do arquivo (campo tiling do descritor T#):

  tiling 8  (Display_LinearAligned)  linear: linhas de `pitch` elementos, sem embaralhamento;
  tiling 13 (Thin_1dThin)            microtiles de 8x8 elementos. Dentro do microtile, blocos BC
                                     (64/128 bits) ficam em ordem de Morton; pixels de 32 bits
                                     seguem a tabela do detiler do shadPS4 (micro_32bpp.comp);
  tiling 14 (Thin_2dThin)            macro-tiling 2D, usado nos alvos de render. NAO e tratado:
                                     esses alvos so teriam conteudo se o port renderizasse.

A ordem dos canais do RGBA8 vem do campo swizzle do descritor, que o dump escreve nas linhas
de textura dos frame_*.txt ("swizzle=RGBA", "BGRA", "RRR1"...). Sem essa informacao vale RGBA.

uso: python tools/decode_frame_textures.py build/frame_dump [filtro-no-nome]
Requer Pillow.
"""
import pathlib
import re
import struct
import sys

from PIL import Image

NAME = re.compile(r"tex_([0-9a-f]+)_(\d+)x(\d+)_p(\d+)_(\w+?)_t(\d+)\.bin$")


def element_index(x, y, bits):
    """Indice do elemento (x, y) dentro de um microtile 8x8, por tamanho de elemento em bits."""
    x0, x1, x2 = x & 1, (x >> 1) & 1, (x >> 2) & 1
    y0, y1, y2 = y & 1, (y >> 1) & 1, (y >> 2) & 1
    order = {
        8: (x0, x1, x2, y1, y0, y2),
        16: (x0, x1, x2, y0, y1, y2),
        32: (x0, x1, y0, x2, y1, y2),
        # Blocos BC (64 e 128 bits): ordem de Morton. Achada por busca entre as 720 ordens
        # possiveis (a de menor descontinuidade entre blocos vizinhos, com folga de 2x).
        64: (x0, y0, x1, y1, x2, y2),
        128: (x0, y0, x1, y1, x2, y2),
    }[bits]
    return sum(bit << i for i, bit in enumerate(order))


def detile(data, width_el, height_el, pitch_el, element_bytes):
    """Devolve os elementos em ordem linear (linha a linha), width_el x height_el."""
    pitch_el = (max(pitch_el, width_el) + 7) & ~7
    tiles_per_row = pitch_el // 8
    lut = [[element_index(x, y, element_bytes * 8) for x in range(8)] for y in range(8)]
    out = bytearray(width_el * height_el * element_bytes)
    for y in range(height_el):
        ty, iy = divmod(y, 8)
        row = lut[iy]
        for x in range(width_el):
            tx, ix = divmod(x, 8)
            src = ((ty * tiles_per_row + tx) * 64 + row[ix]) * element_bytes
            chunk = data[src:src + element_bytes]
            if len(chunk) == element_bytes:
                dst = (y * width_el + x) * element_bytes
                out[dst:dst + element_bytes] = chunk
    return bytes(out)


# Pixels de 32 bits dentro do microtile 8x8: posicao (coluna, linha) do i-esimo pixel gravado.
# Tabela "rmort" de src/video_core/host_shaders/detilers/micro_32bpp.comp do shadPS4.
RMORT_32 = [0x11011000, 0x31213020, 0x13031202, 0x33233222, 0x51415040, 0x71617060, 0x53435242, 0x73637262,
            0x15051404, 0x35253424, 0x17071606, 0x37273626, 0x55455444, 0x75657464, 0x57475646, 0x77677666]


def detile_micro32(data, width, height, pitch):
    """RGBA8 em microtiles 8x8 (tiling 13) -> pixels em ordem linear."""
    pitch = (max(pitch, width) + 7) & ~7
    tiles_per_row = pitch // 8
    where = {}
    for i in range(64):
        packed = (RMORT_32[i >> 2] >> (8 * (i % 4))) & 0xFF
        where[(packed >> 4, packed & 0xF)] = i  # (coluna, linha) -> indice no microtile
    out = bytearray(width * height * 4)
    for y in range(height):
        ty, iy = divmod(y, 8)
        for x in range(width):
            tx, ix = divmod(x, 8)
            src = ((ty * tiles_per_row + tx) * 64 + where[(ix, iy)]) * 4
            chunk = data[src:src + 4]
            if len(chunk) == 4:
                out[(y * width + x) * 4:(y * width + x) * 4 + 4] = chunk
    return bytes(out)


def linear32(data, width, height, pitch):
    """RGBA8 linear (tiling 8): recorta `width` pixels de cada linha de `pitch`."""
    pitch = max(pitch, width)
    out = bytearray(width * height * 4)
    for y in range(height):
        row = data[y * pitch * 4:y * pitch * 4 + width * 4]
        out[y * width * 4:y * width * 4 + len(row)] = row
    return bytes(out)


def apply_swizzle(image, swizzle):
    """Reordena os canais conforme o descritor: cada letra diz de onde sai R, G, B e A."""
    if swizzle in (None, "RGBA") or len(swizzle) != 4:
        return image
    bands = dict(zip("RGBA", image.split()))
    zero = Image.new("L", image.size, 0)
    one = Image.new("L", image.size, 255)
    picked = [bands.get(c, one if c == "1" else zero) for c in swizzle]
    return Image.merge("RGBA", picked)


def load_swizzles(folder):
    """endereco (hex) -> swizzle, lido das linhas de textura dos frame_*.txt do dump."""
    found = {}
    pattern = re.compile(r"@0x([0-9a-f]+) swizzle=(\S+)")
    for path in folder.glob("frame_*.txt"):
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = pattern.search(line)
            if m:
                found.setdefault(m[1], m[2])
    return found


def dds(width, height, fourcc, payload):
    header = struct.pack("<4sIIIIIII44xII4sIIIIIIIIII", b"DDS ", 124, 0x81007, height, width, len(payload), 0, 1,
                         32, 4, fourcc, 0, 0, 0, 0, 0, 0x1000, 0, 0, 0, 0)
    return header + payload


def convert(path, out_dir, swizzles=None):
    m = NAME.search(path.name)
    if not m:
        return None
    width, height, pitch, fmt, tiling = int(m[2]), int(m[3]), int(m[4]), m[5], int(m[6])
    if tiling not in (13, 8):
        return None
    data = path.read_bytes()
    if fmt in ("BC1", "BC3"):
        element = 8 if fmt == "BC1" else 16
        bw, bh = (width + 3) // 4, (height + 3) // 4
        blocks = detile(data, bw, bh, (pitch + 3) // 4, element)
        tmp = out_dir / (path.stem + ".dds")
        tmp.write_bytes(dds(bw * 4, bh * 4, b"DXT1" if fmt == "BC1" else b"DXT5", blocks))
        image = Image.open(tmp).convert("RGBA").crop((0, 0, width, height))
        tmp.unlink()
    elif fmt == "RGBA8":
        pixels = linear32(data, width, height, pitch) if tiling == 8 else detile_micro32(data, width, height, pitch)
        image = apply_swizzle(Image.frombytes("RGBA", (width, height), pixels), (swizzles or {}).get(m[1]))
    else:
        return None
    target = out_dir / (path.stem + ".png")
    image.save(target)
    return target


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    folder = pathlib.Path(sys.argv[1])
    needle = sys.argv[2] if len(sys.argv) > 2 else ""
    out_dir = folder / "png"
    out_dir.mkdir(exist_ok=True)
    done = 0
    swizzles = load_swizzles(folder)
    for path in sorted(folder.glob("tex_*.bin")):
        if needle in path.name:
            try:
                if convert(path, out_dir, swizzles):
                    done += 1
            except Exception as error:  # textura cortada ou formato inesperado
                print(f"falhou {path.name}: {error}")
    print(f"{done} PNG em {out_dir}")


if __name__ == "__main__":
    main()
