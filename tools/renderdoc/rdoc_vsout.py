# Roda dentro do qrenderdoc (--python). Para o evento RDOC_EID de RDOC_FILE, grava as saidas do vertex shader
# (todas as variaveis, como float) dos primeiros RDOC_N vertices em <captura>_vsout<eid>.txt.
import os
import struct
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
N = int(os.environ.get('RDOC_N', '8'))
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_vsout%d.txt' % EID
lines = []


def log(text):
    lines.append(str(text))


try:
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    controller.SetFrameEvent(EID, True)
    state = controller.GetPipelineState()
    refl = state.GetShaderReflection(rd.ShaderStage.Vertex)
    mesh = controller.GetPostVSData(0, 0, rd.MeshDataStage.VSOut)
    log('vsout: %d vertices, stride %d, buffer %s offset %d' % (mesh.numIndices, mesh.vertexByteStride, mesh.vertexResourceId, mesh.vertexByteOffset))
    sigs = list(refl.outputSignature)
    for s in sigs:
        log('  saida %s loc %d comps %d' % (s.varName, s.regIndex, s.compCount))
    data = controller.GetBufferData(mesh.vertexResourceId, mesh.vertexByteOffset, mesh.vertexByteStride * min(N, max(mesh.numIndices, 1)))
    for v in range(min(N, len(data) // max(mesh.vertexByteStride, 1))):
        row = data[v * mesh.vertexByteStride:(v + 1) * mesh.vertexByteStride]
        floats = struct.unpack('<%df' % (len(row) // 4), row)
        log('v%d: %s' % (v, ' '.join('%.4g' % x for x in floats)))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
with open(OUT, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
os._exit(0)
