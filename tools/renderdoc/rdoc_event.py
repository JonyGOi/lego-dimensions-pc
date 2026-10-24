# Roda dentro do qrenderdoc (--python). Abre RDOC_FILE, vai ao evento RDOC_EID e despeja: buffers de
# cada estagio (primeiros dwords como float e hexa), texturas, viewport e o shader de pixel desmontado.
import os
import struct
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_evento%d.txt' % EID
lines = []


def log(text):
    lines.append(str(text))


def flush():
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


try:
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    textures = {t.resourceId: t for t in controller.GetTextures()}
    controller.SetFrameEvent(EID, True)
    state = controller.GetPipelineState()
    for stage, name in ((rd.ShaderStage.Vertex, 'VS'), (rd.ShaderStage.Fragment, 'PS')):
        refl = state.GetShaderReflection(stage)
        log('===== %s %s =====' % (name, refl.resourceId if refl else None))
        for getter, kind in ((state.GetReadWriteResources, 'rw'), (state.GetReadOnlyResources, 'ro'), (state.GetConstantBlocks, 'cb')):
            try:
                used = getter(stage)
            except Exception as e:
                log('  %s: ? (%s)' % (kind, e))
                continue
            for res in used:
                d = res.descriptor
                rid = d.resource
                if rid in textures:
                    t = textures[rid]
                    log('  %s[%s] textura %s %dx%d %s mips %d' % (kind, res.access.index, rid, t.width, t.height, t.format.Name(), t.mips))
                    continue
                if rid == rd.ResourceId.Null():
                    continue
                size = min(int(d.byteSize) if d.byteSize < (1 << 40) else 512, 512)
                try:
                    data = controller.GetBufferData(rid, d.byteOffset, size)
                except Exception as e:
                    log('  %s[%s] buffer %s: ? (%s)' % (kind, res.access.index, rid, e))
                    continue
                log('  %s[%s] buffer %s offset %d tamanho %d:' % (kind, res.access.index, rid, d.byteOffset, d.byteSize))
                words = struct.unpack_from('<%dI' % (len(data) // 4), data)
                floats = struct.unpack_from('<%df' % (len(data) // 4), data)
                for i in range(0, len(words), 4):
                    log('      %3d: %s | %s' % (i, ' '.join('%08x' % w for w in words[i:i + 4]),
                                                ' '.join('%12.5g' % f for f in floats[i:i + 4])))
    try:
        for stage, name in ((rd.ShaderStage.Vertex, 'VS'), (rd.ShaderStage.Fragment, 'PS')):
            for res in state.GetSamplers(stage):
                d = getattr(res, 'sampler', None) or res.descriptor
                log('sampler %s[%s]: U %s V %s W %s | filtro %s | lod %s..%s bias %s | comparacao %s | sem normalizar %s' %
                    (name, res.access.index, d.addressU, d.addressV, d.addressW,
                     d.filter.minify if hasattr(d.filter, 'minify') else d.filter, d.minLOD, d.maxLOD, d.mipBias,
                     d.compareFunction, getattr(d, 'unnormalized', '?')))
    except Exception as e:
        log('samplers: ? (%s)' % e)
    try:
        vp = state.GetViewport(0)
        log('viewport: %s %s %s %s profundidade %s..%s' % (vp.x, vp.y, vp.width, vp.height, vp.minDepth, vp.maxDepth))
    except Exception as e:
        log('viewport: ? (%s)' % e)
    # RDOC_VS=1: desmonta tambem o vertex shader
    stages = [(rd.ShaderStage.Vertex, 'VS')] if os.environ.get('RDOC_VS') == '1' else []
    for stage, name in stages + [(rd.ShaderStage.Fragment, 'PS')]:
        try:
            refl = state.GetShaderReflection(stage)
            targets = controller.GetDisassemblyTargets(True)
            log('===== %s desmontado (%s) =====' % (name, targets[0]))
            log(controller.DisassembleShader(state.GetGraphicsPipelineObject(), refl, targets[0]))
        except Exception as e:
            log('desmontagem: ? (%s)' % e)
    flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
    flush()
flush()
os._exit(0)
