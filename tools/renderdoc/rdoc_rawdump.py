# Roda dentro do qrenderdoc (--python). No evento RDOC_EID de RDOC_FILE grava em <saida>_raw\ os dados crus de
# cada buffer de leitura/escrita e textura do vertex e do pixel shader (vs_rw0.bin, vs_ro0.bin...), para refazer a
# conta do shader fora da GPU.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_raw'
lines = []
try:
    os.makedirs(OUT, exist_ok=True)
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    controller.SetFrameEvent(EID, True)
    state = controller.GetPipelineState()
    textures = {t.resourceId: t for t in controller.GetTextures()}
    for stage, name in ((rd.ShaderStage.Vertex, 'vs'), (rd.ShaderStage.Fragment, 'ps')):
        for i, res in enumerate(state.GetReadWriteResources(stage)):
            d = res.descriptor
            data = controller.GetBufferData(d.resource, d.byteOffset, d.byteSize)
            open(os.path.join(OUT, '%s_rw%d.bin' % (name, i)), 'wb').write(bytes(data))
            lines.append('%s rw%d %s %d bytes' % (name, i, d.resource, len(data)))
        for i, res in enumerate(state.GetReadOnlyResources(stage)):
            d = res.descriptor
            t = textures.get(d.resource)
            if not t:
                continue
            data = controller.GetTextureData(d.resource, rd.Subresource(0, 0, 0))
            open(os.path.join(OUT, '%s_ro%d.bin' % (name, i)), 'wb').write(bytes(data))
            lines.append('%s ro%d %s %dx%d %s %d bytes' % (name, i, d.resource, t.width, t.height, t.format.Name(), len(data)))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    lines.append('ERRO\n' + traceback.format_exc())
lines.append('script terminou')
with open(OUT + '.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
os._exit(0)
