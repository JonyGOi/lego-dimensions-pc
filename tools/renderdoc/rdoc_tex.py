# Roda dentro do qrenderdoc (--python). Abre RDOC_FILE, vai ao evento RDOC_EID e, para cada textura
# de RDOC_TEXS ("id,id"), grava minimo / maximo, uma grade de amostras e um PNG.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
WANT = os.environ['RDOC_TEXS'].split(',')
BASE = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0])
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(BASE + '_tex.txt', 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    controller.SetFrameEvent(EID, True)
    sub = rd.Subresource(0, 0, 0)
    for t in controller.GetTextures():
        ident = str(t.resourceId).split('::')[-1]
        if ident not in WANT:
            continue
        log('===== %s %dx%d %s mips %d =====' % (t.resourceId, t.width, t.height, t.format.Name(), t.mips))
        mn, mx = controller.GetMinMax(t.resourceId, sub, rd.CompType.Typeless)
        log('minimo %s' % list(mn.floatValue)[:4])
        log('maximo %s' % list(mx.floatValue)[:4])
        for iy in range(8):
            row = []
            for ix in range(8):
                v = controller.PickPixel(t.resourceId, int((ix + 0.5) * t.width / 8), int((iy + 0.5) * t.height / 8), sub, rd.CompType.Typeless).floatValue
                row.append('%6.2f/%6.2f/%6.2f/%5.2f' % (v[0], v[1], v[2], v[3]))
            log('  ' + '  '.join(row))
        save = rd.TextureSave()
        save.resourceId = t.resourceId
        save.destType = rd.FileType.PNG
        save.mip = 0
        save.alpha = rd.AlphaMapping.Discard
        save.comp.blackPoint = 0.0
        save.comp.whitePoint = max(1.0, max(list(mx.floatValue)[:3]))
        controller.SaveTexture(save, BASE + '_tex%s.png' % ident)
    flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
    flush()
flush()
os._exit(0)
