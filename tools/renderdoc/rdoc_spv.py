# Roda dentro do qrenderdoc (--python). Grava o SPIR-V do VS e do PS do evento RDOC_EID de RDOC_FILE em
# <captura>_e<eid>_vs.spv / _ps.spv (para achar o par do port com o mesmo modulo).
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
BASE = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_e%d' % EID
lines = []
try:
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    controller.SetFrameEvent(EID, True)
    state = controller.GetPipelineState()
    for stage, name in ((rd.ShaderStage.Vertex, 'vs'), (rd.ShaderStage.Fragment, 'ps')):
        refl = state.GetShaderReflection(stage)
        data = bytes(refl.rawBytes)
        with open(BASE + '_%s.spv' % name, 'wb') as f:
            f.write(data)
        lines.append('%s: %d bytes' % (name, len(data)))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    lines.append('ERRO\n' + traceback.format_exc())
lines.append('script terminou')
with open(BASE + '_spv.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
os._exit(0)
