# Roda dentro do qrenderdoc (--python). Historico de pixels EXATOS num alvo:
#   RDOC_FILE = captura; RDOC_TEX = id do alvo (o numero depois de "::"); RDOC_EID = ultimo evento;
#   RDOC_POINTS = "x,y;x,y;..." (pixels do alvo). Para cada modificacao: saida do shader, se passou
#   (e por que nao), blend, profundidade, shaders e texturas do draw. Tambem salva o alvo em PNG.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_ponto.txt'
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}
    target = None
    for rid in textures:
        if str(rid).endswith('::' + os.environ['RDOC_TEX']):
            target = rid
    eid = int(os.environ['RDOC_EID'])
    sub = rd.Subresource(0, 0, 0)
    controller.SetFrameEvent(eid, True)
    save = rd.TextureSave()
    save.resourceId = target
    save.alpha = rd.AlphaMapping.Preserve
    save.destType = rd.FileType.PNG
    controller.SaveTexture(save, os.path.splitext(OUT)[0] + '_alvo.png')
    save.alpha = rd.AlphaMapping.Discard
    controller.SaveTexture(save, os.path.splitext(OUT)[0] + '_alvo_rgb.png')
    described = set()
    for item in os.environ['RDOC_POINTS'].split(';'):
        x, y = (int(v) for v in item.split(','))
        v = controller.PickPixel(target, x, y, sub, rd.CompType.Typeless).floatValue
        log('')
        log('===== pixel (%d, %d) no evento %d = %.3f %.3f %.3f %.3f =====' % (x, y, eid, v[0], v[1], v[2], v[3]))
        history = [m for m in controller.PixelHistory(target, x, y, sub, rd.CompType.Typeless) if m.eventId <= eid]
        for m in history:
            pre, post, out = m.preMod.col.floatValue, m.postMod.col.floatValue, m.shaderOut.col.floatValue
            why = 'passou' if m.Passed() else 'FALHOU (%s%s%s%s)' % ('profundidade ' if m.depthTestFailed else '', 'estencil ' if m.stencilTestFailed else '',
                                                                   'descartado ' if m.shaderDiscarded else '', 'scissor' if m.scissorClipped else '')
            log('  evento %d prim %d: %s | saida %.3f %.3f %.3f %.3f | antes %.3f %.3f %.3f %.3f -> depois %.3f %.3f %.3f %.3f | prof %.5f -> %.5f' %
                (m.eventId, m.primitiveID, why, out[0], out[1], out[2], out[3], pre[0], pre[1], pre[2], pre[3], post[0], post[1], post[2], post[3],
                 m.preMod.depth, m.postMod.depth) + ' | prof do shader %.7f' % m.shaderOut.depth)
            if m.eventId in described:
                continue
            described.add(m.eventId)
            controller.SetFrameEvent(m.eventId, True)
            state = controller.GetPipelineState()
            try:
                b = state.GetColorBlends()[0]
                log('      blend %s: cor %s %s %s | alfa %s %s %s | mascara %s' % (b.enabled, b.colorBlend.source, b.colorBlend.operation,
                                                                                 b.colorBlend.destination, b.alphaBlend.source, b.alphaBlend.operation,
                                                                                 b.alphaBlend.destination, b.writeMask))
            except Exception as e:
                log('      blend ? %s' % e)
            try:
                ds = state.GetDepthState() if hasattr(state, 'GetDepthState') else None
                if ds is not None:
                    log('      profundidade: teste %s escrita %s funcao %s' % (ds.depthEnable, ds.depthWrites, ds.depthFunction))
            except Exception as e:
                log('      profundidade ? %s' % e)
            vs = state.GetShaderReflection(rd.ShaderStage.Vertex)
            ps = state.GetShaderReflection(rd.ShaderStage.Fragment)
            log('      VS %s PS %s' % (vs.resourceId if vs else None, ps.resourceId if ps else None))
            for res in state.GetReadOnlyResources(rd.ShaderStage.Fragment):
                rid = res.descriptor.resource
                t = textures.get(rid)
                if t:
                    log('      textura binding %s: %s %dx%d %s mips %d %s' % (res.access.index, rid, t.width, t.height, t.format.Name(), t.mips,
                                                                         'ALVO' if t.creationFlags & rd.TextureCategory.ColorTarget else ''))
            controller.SetFrameEvent(eid, True)
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
flush()
os._exit(0)
