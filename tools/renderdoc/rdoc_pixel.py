# Roda dentro do qrenderdoc (--python). Abre RDOC_FILE e responde "quem pintou este pixel":
#   - acha o ultimo alvo de cor grande desenhado (ou RDOC_TEX = id do recurso, RDOC_EID = evento);
#   - procura o pixel mais claro na regiao RDOC_REGION = "x0,y0,x1,y1" (fracoes 0..1 do alvo);
#   - imprime o historico do pixel e, para cada draw que o alterou, blend, texturas e o valor delas
#     na mesma posicao relativa.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_pixel.txt'
REGION = [float(v) for v in os.environ.get('RDOC_REGION', '0.55,0.6,0.95,0.95').split(',')]
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    opened = cap.OpenCapture(rd.ReplayOptions(), None)
    controller = opened[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}

    def flat(actions, out):
        for a in actions:
            out.append(a)
            flat(a.children, out)
        return out

    actions = [a for a in flat(controller.GetRootActions(), []) if a.flags & rd.ActionFlags.Drawcall]
    log('%d draws' % len(actions))

    def desc(rid):
        t = textures.get(rid)
        return '%s %dx%d %s' % (rid, t.width, t.height, t.format.Name()) if t else str(rid)

    # ultimo alvo de cor 1920x1080 (que nao seja o swapchain: o penultimo "grupo")
    final_tex, final_eid = None, 0
    if os.environ.get('RDOC_TEX'):
        wanted = os.environ['RDOC_TEX']
        for rid in textures:
            if str(rid).endswith('::' + wanted):
                final_tex = rid
        final_eid = int(os.environ.get('RDOC_EID', actions[-1].eventId))
    else:
        seen = []
        for a in actions:
            rid = a.outputs[0]
            t = textures.get(rid)
            if t and t.width >= 1900 and (not seen or seen[-1][0] != rid):
                seen.append([rid, a.eventId])
            elif t and t.width >= 1900:
                seen[-1][1] = a.eventId
        for rid, eid in seen:
            log('alvo grande: %s ate o evento %d' % (desc(rid), eid))
        final_tex, final_eid = seen[-2] if len(seen) > 1 else seen[-1]
    sub = rd.Subresource(0, 0, 0)
    by_eid = {a.eventId: a for a in actions}
    fx = fy = None
    visited = set()
    for level in range(int(os.environ.get('RDOC_LEVELS', '7'))):
        tex = textures[final_tex]
        log('')
        log('===== nivel %d: %s no evento %d =====' % (level, desc(final_tex), final_eid))
        controller.SetFrameEvent(final_eid, True)
        if fx is None:
            best, where = -1.0, (0, 0)
            for iy in range(24):
                for ix in range(32):
                    x = int((REGION[0] + (REGION[2] - REGION[0]) * ix / 31.0) * (tex.width - 1))
                    y = int((REGION[1] + (REGION[3] - REGION[1]) * iy / 23.0) * (tex.height - 1))
                    v = controller.PickPixel(final_tex, x, y, sub, rd.CompType.Typeless).floatValue
                    # RDOC_PICK=branco: o pixel mais branco (maior minimo entre R, G e B), ex. fumaca
                    score = min(v[0], v[1], v[2]) if os.environ.get('RDOC_PICK') == 'branco' else v[0] + v[1] + v[2]
                    if score > best:
                        best, where = score, (x, y)
            x, y = where
            fx, fy = x / float(tex.width), y / float(tex.height)
        x, y = min(int(fx * tex.width), tex.width - 1), min(int(fy * tex.height), tex.height - 1)
        v = controller.PickPixel(final_tex, x, y, sub, rd.CompType.Typeless).floatValue
        log('pixel (%d, %d) = %.3f %.3f %.3f %.3f' % (x, y, v[0], v[1], v[2], v[3]))
        history = [m for m in controller.PixelHistory(final_tex, x, y, sub, rd.CompType.Typeless) if m.eventId <= final_eid]
        passed = [m for m in history if m.Passed()]
        log('historico: %d modificacoes, %d passaram' % (len(history), len(passed)))
        biggest, biggest_gain = None, -1e9
        for mod in passed[-int(os.environ.get('RDOC_SHOW', '14')):]:
            pre, post, out = mod.preMod.col.floatValue, mod.postMod.col.floatValue, mod.shaderOut.col.floatValue
            gain = (post[0] + post[1] + post[2]) - (pre[0] + pre[1] + pre[2])
            if gain >= biggest_gain:
                biggest, biggest_gain = mod, gain
            log('  evento %d | antes %.3f %.3f %.3f %.3f | saida do shader %.3f %.3f %.3f %.3f | depois %.3f %.3f %.3f %.3f' %
                (mod.eventId, pre[0], pre[1], pre[2], pre[3], out[0], out[1], out[2], out[3], post[0], post[1], post[2], post[3]))
        if not biggest:
            break
        controller.SetFrameEvent(biggest.eventId, True)
        state = controller.GetPipelineState()
        a = by_eid.get(biggest.eventId)
        log('--- maior ganho: evento %d (%s indices, %s instancias)' % (biggest.eventId, a.numIndices if a else '?', a.numInstances if a else '?'))
        try:
            b = state.GetColorBlends()[0]
            log('    blend: ligado %s | cor %s %s %s | alfa %s %s %s' %
                (b.enabled, b.colorBlend.source, b.colorBlend.operation, b.colorBlend.destination, b.alphaBlend.source, b.alphaBlend.operation,
                 b.alphaBlend.destination))
        except Exception as e:
            log('    blend: ? (%s)' % e)
        nxt, nxt_value = None, -1.0
        try:
            frag = state.GetShaderReflection(rd.ShaderStage.Fragment)
            log('    PS: %s' % (frag.resourceId if frag else None))
            for res in state.GetReadOnlyResources(rd.ShaderStage.Fragment):
                rid = res.descriptor.resource
                t = textures.get(rid)
                if not t:
                    continue
                px, py = min(int(fx * t.width), t.width - 1), min(int(fy * t.height), t.height - 1)
                try:
                    pv = controller.PickPixel(rid, px, py, sub, rd.CompType.Typeless).floatValue
                    value = '%.3f %.3f %.3f %.3f' % (pv[0], pv[1], pv[2], pv[3])
                    lum = pv[0] + pv[1] + pv[2]
                except Exception:
                    value, lum = '?', -1.0
                is_target = bool(t.creationFlags & rd.TextureCategory.ColorTarget)
                log('    textura (binding %s): %s mips %d %s | na mesma posicao relativa: %s' %
                    (res.access.index, desc(rid), t.mips, 'ALVO' if is_target else 'textura', value))
                if is_target and rid != final_tex and (rid, biggest.eventId) not in visited and lum > nxt_value:
                    nxt, nxt_value = rid, lum
        except Exception as e:
            log('    recursos: ? (%s)' % e)
        if nxt is None:
            break
        visited.add((nxt, biggest.eventId))
        final_tex, final_eid = nxt, biggest.eventId - 1
    flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
    flush()
flush()
os._exit(0)
