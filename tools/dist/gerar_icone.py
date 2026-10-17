#!/usr/bin/env python3
"""Fase 50: icone proprio do port (desenho original: portal em redemoinho + peca de montar amarela).
Gera tools/dist/app.ico (16 a 256 px) e app_256.png. Nao usa nenhuma arte do jogo nem o logo da LEGO.
uso: python tools/dist/gerar_icone.py"""
import math
import os

from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
S = 1024  # desenha grande e reduz (antialias)


def lerp(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(len(a)))


def base():
    img = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    # fundo: quadrado arredondado com degrade azul-marinho -> roxo
    bg = Image.new('RGBA', (S, S))
    px = bg.load()
    for y in range(S):
        for x in range(S):
            t = (x + y) / (2 * S)
            px[x, y] = lerp((14, 24, 64, 255), (74, 22, 110, 255), t)
    mask = Image.new('L', (S, S), 0)
    ImageDraw.Draw(mask).rounded_rectangle((24, 24, S - 24, S - 24), radius=210, fill=255)
    img.paste(bg, (0, 0), mask)
    # portal: aneis em espiral (brilho ciano)
    glow = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(glow)
    cx, cy = S / 2, S / 2 + 10
    for k in range(4):
        r0 = 420 - k * 75
        for a in range(0, 360, 2):
            t = math.radians(a + k * 47)
            r = r0 - 30 * (a / 360)
            x, y = cx + r * math.cos(t), cy + r * math.sin(t) * 0.92
            w = 46 - k * 5
            c = lerp((90, 220, 255), (40, 90, 255), k / 5)
            d.ellipse((x - w / 2, y - w / 2, x + w / 2, y + w / 2), fill=c + (int(150 + 100 * (a / 360)),))
    glow = glow.filter(ImageFilter.GaussianBlur(6))
    img.alpha_composite(glow)
    # centro escuro do portal
    core = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    ImageDraw.Draw(core).ellipse((cx - 200, cy - 190, cx + 200, cy + 190), fill=(8, 10, 30, 210))
    img.alpha_composite(core.filter(ImageFilter.GaussianBlur(30)))
    # peca 2x2 em perspectiva (amarela)
    brick = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(brick)
    w, h, depth = 400, 200, 160
    x0, y0 = cx - w / 2, cy - 30
    top = [(x0, y0), (x0 + w / 2, y0 - h / 2), (x0 + w, y0), (x0 + w / 2, y0 + h / 2)]
    left = [(x0, y0), (x0 + w / 2, y0 + h / 2), (x0 + w / 2, y0 + h / 2 + depth), (x0, y0 + depth)]
    right = [(x0 + w, y0), (x0 + w / 2, y0 + h / 2), (x0 + w / 2, y0 + h / 2 + depth), (x0 + w, y0 + depth)]
    d.polygon(left, fill=(222, 160, 0, 255))
    d.polygon(right, fill=(186, 122, 0, 255))
    d.polygon(top, fill=(255, 205, 20, 255))
    # pinos (studs)
    for i in range(2):
        for j in range(2):
            u, v = 0.27 + 0.46 * i, 0.27 + 0.46 * j
            px_ = x0 + (u + v) * w / 2
            py_ = y0 + (v - u) * h / 2
            rx, ry, sh = 56, 28, 34
            d.rectangle((px_ - rx, py_ - sh, px_ + rx, py_), fill=(232, 172, 0, 255))
            d.ellipse((px_ - rx, py_ - ry, px_ + rx, py_ + ry), fill=(232, 172, 0, 255))
            d.ellipse((px_ - rx, py_ - sh - ry, px_ + rx, py_ - sh + ry), fill=(255, 222, 70, 255))
    shadow = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    ImageDraw.Draw(shadow).ellipse((cx - 190, cy + 170, cx + 190, cy + 230), fill=(0, 0, 0, 120))
    img.alpha_composite(shadow.filter(ImageFilter.GaussianBlur(18)))
    img.alpha_composite(brick)
    # borda clara sutil
    ring = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    ImageDraw.Draw(ring).rounded_rectangle((24, 24, S - 24, S - 24), radius=210, outline=(255, 255, 255, 50), width=10)
    img.alpha_composite(ring)
    return img


def main():
    img = base()
    sizes = [256, 128, 64, 48, 32, 24, 16]
    frames = [img.resize((n, n), Image.LANCZOS) for n in sizes]
    frames[0].save(os.path.join(HERE, 'app_256.png'))
    frames[0].save(os.path.join(HERE, 'app.ico'), sizes=[(n, n) for n in sizes], append_images=frames[1:])
    print('ok: app.ico + app_256.png')


if __name__ == '__main__':
    main()
