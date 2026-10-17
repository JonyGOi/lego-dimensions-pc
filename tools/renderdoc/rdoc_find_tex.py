# Roda dentro do qrenderdoc (--python). Abre RDOC_FILE e procura os draws cujo PS le uma textura com o tamanho
# RDOC_SIZE (ex.: "200x200"). Para cada um, lista as texturas do PS e salva (EXR e PNG) as de tamanho RDOC_SAVE_SIZES
# (ex.: "200x200,64x64") daquele evento. Saida: <captura>_achados.txt e <captura>_e<eid>_t<n>.exr/png.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
WANT = os.environ.get('RDOC_SIZE', '200x200')
SAVE_SIZES = set(os.environ.get('RDOC_SAVE_SIZES', '200x200,64x64').split(','))
MAX_HITS = int(os.environ.get('RDOC_MAX_HITS', '4'))
BASE = os.path.splitext(PATH)[0]
OUT = BASE + '_achados.txt'
lines = []


def log(text):
    lines.append(str(text))
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


def save(controller, rid, path, ftype):
    s = rd.TextureSave()
    s.resourceId = rid
    s.mip = 0
    s.slice.sliceIndex = 0
    s.alpha = rd.AlphaMapping.Preserve
    s.destType = ftype
    controller.SaveTexture(s, path)


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}
    draws = []

    def walk(action):
        if action.flags & rd.ActionFlags.Drawcall:
            draws.append(action)
        for child in action.children:
            walk(child)

    for root in controller.GetRootActions():
        walk(root)
    log('%d draws' % len(draws))
    hits = 0
    for a in draws:
        if a.numIndices > 6:
            continue
        controller.SetFrameEvent(a.eventId, False)
        state = controller.GetPipelineState()
        try:
            used = state.GetReadOnlyResources(rd.ShaderStage.Fragment)
        except Exception:
            continue
        texs = []
        for res in used:
            t = textures.get(res.descriptor.resource)
            if t is not None:
                texs.append((res.access.index, t))
        sizes = ['%dx%d' % (t.width, t.height) for _, t in texs]
        if WANT not in sizes:
            continue
        hits += 1
        outs = state.GetOutputTargets()
        log('evento %d: alvos %s | texturas do PS: %s' % (
            a.eventId, ', '.join('%s' % o.resource for o in outs if o.resource != rd.ResourceId.Null()),
            ' ; '.join('[%s] %s %dx%d %s mips %d' % (i, t.resourceId, t.width, t.height, t.format.Name(), t.mips) for i, t in texs)))
        for i, t in texs:
            if '%dx%d' % (t.width, t.height) in SAVE_SIZES:
                for ext, ftype in (('exr', rd.FileType.EXR), ('png', rd.FileType.PNG)):
                    p = '%s_e%d_t%s.%s' % (BASE, a.eventId, i, ext)
                    try:
                        save(controller, t.resourceId, p, ftype)
                    except Exception as e:
                        log('  salvar %s: %s' % (p, e))
                log('  salvo t%s %dx%d %s' % (i, t.width, t.height, t.format.Name()))
        if hits >= MAX_HITS:
            break
    log('achados: %d' % hits)
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
os._exit(0)
