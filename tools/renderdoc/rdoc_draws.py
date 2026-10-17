# Roda dentro do qrenderdoc (--python). Para cada evento de RDOC_EIDS ("e1,e2,..."): indices, instancias,
# estado de profundidade (Vulkan), buffers de vertices/indices e shaders. Serve para comparar o pre-passe
# de profundidade com o passe de cor do mesmo objeto.
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_draws.txt'
lines = []


def log(text):
    lines.append(str(text))


try:
    cap = rd.OpenCaptureFile()
    cap.OpenFile(PATH, '', None)
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]

    def flat(actions, out):
        for a in actions:
            out.append(a)
            flat(a.children, out)
        return out

    by_eid = {a.eventId: a for a in flat(controller.GetRootActions(), [])}
    for eid in [int(x) for x in os.environ['RDOC_EIDS'].split(',')]:
        a = by_eid.get(eid)
        controller.SetFrameEvent(eid, True)
        vk = controller.GetVulkanPipelineState()
        ds = vk.depthStencil
        log('evento %d: %s indices x %s instancias, base vertex %s, primeiro indice %s' %
            (eid, a.numIndices if a else '?', a.numInstances if a else '?', a.baseVertex if a else '?', a.indexOffset if a else '?'))
        log('    profundidade: teste %s escrita %s funcao %s | estencil %s' % (ds.depthTestEnable, ds.depthWriteEnable, ds.depthFunction,
                                                                          ds.stencilTestEnable))
        log('    raster: cull %s clamp %s bias %s' % (vk.rasterizer.cullMode, vk.rasterizer.depthClampEnable, vk.rasterizer.depthBiasEnable))
        ib = vk.inputAssembly.indexBuffer
        log('    indices: %s offset %s' % (ib.resourceId, ib.byteOffset))
        for i, vb in enumerate(vk.vertexInput.vertexBuffers):
            log('    vertices %d: %s offset %s stride %s' % (i, vb.resourceId, vb.byteOffset, vb.byteStride))
        for at in vk.vertexInput.attributes:
            log('    atributo loc %s binding %s formato %s offset %s' % (at.location, at.binding, at.format.Name(), at.byteOffset))
        log('    VS %s PS %s' % (vk.vertexShader.resourceId, vk.fragmentShader.resourceId))
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
with open(OUT, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
os._exit(0)
