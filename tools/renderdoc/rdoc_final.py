# Roda dentro do qrenderdoc (--python). Abre RDOC_FILE e grava em PNG o estado final de cada alvo de cor grande
# (>= 1900 de largura) do quadro, na ordem em que foram desenhados, com a lista em <captura>_final.txt.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
BASE = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0])
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(BASE + '_final.txt', 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}

    def flat(actions, out):
        for a in actions:
            out.append(a)
            flat(a.children, out)
        return out

    actions = [a for a in flat(controller.GetRootActions(), []) if a.flags & rd.ActionFlags.Drawcall]
    last = {}
    for a in actions:
        t = textures.get(a.outputs[0])
        if t and t.width >= 1900:
            last[a.outputs[0]] = a.eventId
    sub = rd.Subresource(0, 0, 0)
    for rid, eid in sorted(last.items(), key=lambda x: x[1]):
        t = textures[rid]
        controller.SetFrameEvent(eid, True)
        mn, mx = controller.GetMinMax(rid, sub, rd.CompType.Typeless)
        ident = str(rid).split('::')[-1]
        log('%s %dx%d %s ultimo evento %d max %s' % (rid, t.width, t.height, t.format.Name(), eid, list(mx.floatValue)[:3]))
        save = rd.TextureSave()
        save.resourceId = rid
        save.destType = rd.FileType.PNG
        save.alpha = rd.AlphaMapping.Discard
        save.comp.blackPoint = 0.0
        save.comp.whitePoint = 1.0
        controller.SaveTexture(save, BASE + '_final%s.png' % ident)
    log('script terminou')
    flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
    log('script terminou')
    flush()
os._exit(0)
