# Roda dentro do qrenderdoc (--python). Acha em RDOC_FILE os draws cujo vertex shader le uma textura de tamanho e
# formato RDOC_VSTEX ("256x128:R32G32B32A32_FLOAT") e, para cada um, o maior valor de cada saida do VS (post-VS),
# em <saida>_vstex.txt. Serve para comparar o mesmo shader entre o port e o shadPS4.
import os
import struct
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_vstex.txt'
size, fmt = os.environ.get('RDOC_VSTEX', '256x128:R32G32B32A32_FLOAT').split(':')
W, H = [int(v) for v in size.split('x')]
LIMIT = int(os.environ.get('RDOC_MAX', '12'))
lines = []


def log(text):
    lines.append(str(text))


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
    found = 0
    for a in actions:
        controller.SetFrameEvent(a.eventId, False)
        state = controller.GetPipelineState()
        hit = False
        for res in state.GetReadOnlyResources(rd.ShaderStage.Vertex):
            d = res.descriptor
            t = textures.get(d.resource)
            if t and t.width == W and t.height == H and t.format.Name() == fmt:
                hit = True
        if not hit:
            continue
        found += 1
        mesh = controller.GetPostVSData(0, 0, rd.MeshDataStage.VSOut)
        n = min(mesh.numIndices, 64)
        data = controller.GetBufferData(mesh.vertexResourceId, mesh.vertexByteOffset, mesh.vertexByteStride * n)
        stride = mesh.vertexByteStride
        comps = stride // 4
        mx = [0.0] * comps
        for v in range(len(data) // stride if stride else 0):
            fl = struct.unpack_from('<%df' % comps, data, v * stride)
            for i, x in enumerate(fl):
                if x == x and abs(x) > abs(mx[i]):
                    mx[i] = x
        log('evento %d: %d indices, stride %d, maiores por componente: %s' % (a.eventId, a.numIndices, stride,
                                                                            ' '.join('%.3g' % x for x in mx)))
        if found >= LIMIT:
            break
    log('achados: %d' % found)
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
with open(OUT, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
os._exit(0)
