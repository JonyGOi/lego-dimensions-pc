# Roda dentro do qrenderdoc (--python). Para os eventos RDOC_EIDS da captura RDOC_FILE, lista os
# buffers e as texturas ligados ao vertex e ao pixel shader, com os primeiros floats de cada buffer.
import os
import struct
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
LOG = os.environ['RDOC_OUT'] + '_bufs.txt'
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(LOG, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}
    buffers = {b.resourceId: b for b in controller.GetBuffers()}
    names = {r.resourceId: r.name for r in controller.GetResources()}
    for eid in [int(x) for x in os.environ['RDOC_EIDS'].split(',')]:
        controller.SetFrameEvent(eid, True)
        state = controller.GetPipelineState()
        log('===== evento %d' % eid)
        for stage, label in ((rd.ShaderStage.Vertex, 'VS'), (rd.ShaderStage.Pixel, 'PS')):
            refl = state.GetShaderReflection(stage)
            log('-- %s shader %s' % (label, names.get(state.GetShader(stage), '?')))
            groups = []
            for getter in ('GetConstantBlocks', 'GetReadOnlyResources', 'GetReadWriteResources', 'GetSamplers'):
                try:
                    groups.append((getter, getattr(state, getter)(stage)))
                except Exception as error:
                    log('   %s falhou: %s' % (getter, error))
            for getter, used in groups:
                for item in used:
                    d = getattr(item, 'descriptor', item)
                    access = getattr(item, 'access', None)
                    index = getattr(access, 'index', '?') if access else '?'
                    rid = getattr(d, 'resource', None)
                    if rid is None:
                        log('   %s idx %s: %s' % (getter, index, d))
                        continue
                    if rid in buffers:
                        offset = getattr(d, 'byteOffset', 0)
                        size = getattr(d, 'byteSize', 0)
                        take = min(size if size and size < (1 << 40) else 256, 512)
                        data = controller.GetBufferData(rid, offset, take)
                        count = len(data) // 4
                        floats = struct.unpack('<%df' % count, bytes(data[:count * 4]))
                        log('   %s idx %s: buffer %s offset %d tamanho %d' % (getter, index, names.get(rid, rid), offset, size))
                        for row in range(0, min(count, 48), 8):
                            log('        [%2d] %s' % (row, ' '.join('%.5g' % v for v in floats[row:row + 8])))
                    elif rid in textures:
                        t = textures[rid]
                        log('   %s idx %s: textura %s [%dx%d d%d %s mips %d arr %d] primeiro mip %s' % (
                            getter, index, names.get(rid, rid), t.width, t.height, t.depth, t.format.Name(), t.mips, t.arraysize,
                            getattr(d, 'firstMip', '?')))
                    else:
                        log('   %s idx %s: recurso %s' % (getter, index, rid))
        flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
flush()
