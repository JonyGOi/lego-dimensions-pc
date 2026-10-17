# Roda dentro do qrenderdoc (--python). Abre a captura RDOC_FILE e escreve a estrutura de passes:
# grupos de draws seguidos com os mesmos alvos, com tamanho e formato de cada alvo.
# RDOC_SAVE = lista "eid:idx,eid:idx" -> salva o alvo de cor idx (ou 'd' = profundidade) depois do evento.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0])
LOG = OUT + '_passes.txt'
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(LOG, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    result = cap.OpenFile(PATH, '', None)
    log('OpenFile: %s' % result)
    opened = cap.OpenCapture(rd.ReplayOptions(), None)
    controller = opened[1]
    log('OpenCapture: %s' % opened[0])
    textures = {t.resourceId: t for t in controller.GetTextures()}
    names = {}
    try:
        for res in controller.GetResources():
            names[res.resourceId] = res.name
    except Exception:
        pass

    def describe(rid):
        if rid == rd.ResourceId.Null():
            return '-'
        t = textures.get(rid)
        if t is None:
            return str(rid)
        return '%s[%dx%d %s m%d a%d]' % (names.get(rid, str(rid)), t.width, t.height, t.format.Name(), t.mips, t.arraysize)

    flat = []

    def walk(action):
        if action.flags & rd.ActionFlags.Drawcall or action.flags & rd.ActionFlags.Clear:
            flat.append(action)
        for child in action.children:
            walk(child)

    for root in controller.GetRootActions():
        walk(root)
    log('%d acoes de draw / clear' % len(flat))
    group = None
    for action in flat:
        key = (tuple(str(o) for o in action.outputs), str(action.depthOut))
        if group and group['key'] == key:
            group['last'] = action.eventId
            group['count'] += 1
            group['indices'] += action.numIndices
        else:
            if group:
                log('eid %6d..%6d  %5d draws %9d indices  cor: %s | prof: %s' % (group['first'], group['last'], group['count'], group['indices'],
                                                                              ', '.join(describe(o) for o in group['outs'] if o != rd.ResourceId.Null()),
                                                                              describe(group['depth'])))
            group = {'key': key, 'first': action.eventId, 'last': action.eventId, 'count': 1, 'indices': action.numIndices,
                     'outs': list(action.outputs), 'depth': action.depthOut}
    if group:
        log('eid %6d..%6d  %5d draws %9d indices  cor: %s | prof: %s' % (group['first'], group['last'], group['count'], group['indices'],
                                                                      ', '.join(describe(o) for o in group['outs'] if o != rd.ResourceId.Null()),
                                                                      describe(group['depth'])))
    flush()
    by_eid = {a.eventId: a for a in flat}
    for item in [x for x in os.environ.get('RDOC_SAVE', '').split(',') if x]:
        eid_text, index = item.split(':')
        eid = int(eid_text)
        action = by_eid.get(eid)
        if action is None:
            log('salvar: evento %d nao e um draw' % eid)
            continue
        controller.SetFrameEvent(eid, True)
        rid = action.depthOut if index == 'd' else action.outputs[int(index)]
        t = textures.get(rid)
        base = '%s_e%d_%s' % (OUT, eid, index)
        save = rd.TextureSave()
        save.resourceId = rid
        save.mip = 0
        save.slice.sliceIndex = 0
        save.alpha = rd.AlphaMapping.Preserve
        save.destType = rd.FileType.EXR
        ok = controller.SaveTexture(save, base + '.exr')
        save.destType = rd.FileType.PNG
        ok2 = controller.SaveTexture(save, base + '.png')
        try:
            mn, mx = controller.GetMinMax(rid, rd.Subresource(0, 0, 0), rd.CompType.Typeless)
            extra = 'min %s max %s' % ([round(v, 4) for v in mn.floatValue[:4]], [round(v, 4) for v in mx.floatValue[:4]])
        except Exception as error:
            extra = 'minmax falhou: %s' % error
        log('salvo eid %d alvo %s %s -> %s (exr %s, png %s) %s' % (eid, index, describe(rid), base, ok, ok2, extra))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
flush()
