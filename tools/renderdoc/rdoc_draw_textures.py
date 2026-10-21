# Roda dentro do qrenderdoc (--python). Lista, para cada draw entre RDOC_FROM e RDOC_TO, as texturas que o PS le
# (tamanho e formato) e o numero de indices. Salva em PNG as texturas maiores que RDOC_MIN_SIDE (padrao 256).
# Saida: <captura>_texturas.txt e <captura>_t<resourceid>.png.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
FROM = int(os.environ.get('RDOC_FROM', '0'))
TO = int(os.environ.get('RDOC_TO', '999999'))
MIN_SIDE = int(os.environ.get('RDOC_MIN_SIDE', '256'))
BASE = os.path.splitext(PATH)[0]
OUT = BASE + '_texturas.txt'
lines = []


def log(text):
    lines.append(str(text))
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}
    draws = []

    def walk(action):
        if action.flags & rd.ActionFlags.Drawcall and FROM <= action.eventId <= TO:
            draws.append(action)
        for child in action.children:
            walk(child)

    for root in controller.GetRootActions():
        walk(root)
    saved = set()
    for a in draws:
        controller.SetFrameEvent(a.eventId, False)
        state = controller.GetPipelineState()
        texs = []
        for res in state.GetReadOnlyResources(rd.ShaderStage.Fragment):
            t = textures.get(res.descriptor.resource)
            if t is not None:
                texs.append(t)
        log('evento %d: %d indices | %s' % (a.eventId, a.numIndices,
                                            ' ; '.join('%s %dx%d %s m%d' % (t.resourceId, t.width, t.height, t.format.Name(), t.mips) for t in texs)))
        for t in texs:
            if max(t.width, t.height) >= MIN_SIDE and t.resourceId not in saved:
                saved.add(t.resourceId)
                s = rd.TextureSave()
                s.resourceId = t.resourceId
                s.mip = 0
                s.slice.sliceIndex = 0
                s.alpha = rd.AlphaMapping.Preserve
                s.destType = rd.FileType.PNG
                rid = str(t.resourceId).split('::')[-1]
                controller.SaveTexture(s, '%s_t%s.png' % (BASE, rid))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
os._exit(0)
