# Roda dentro do qrenderdoc (--python). Le so os dados estruturados da captura RDOC_FILE (sem replay) e, para
# cada pixel shader em RDOC_SPV_DIR (*.ps.spv), lista os pipelines criados com ele e as flags de criacao
# (DISABLE_OPTIMIZATION = 0x1). Saida: RDOC_OUT.
import glob
import hashlib
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
SPV_DIR = os.environ['RDOC_SPV_DIR']
OUT = os.environ['RDOC_OUT']
lines = []


def log(text):
    lines.append(str(text))


def child(obj, name):
    for i in range(obj.NumChildren()):
        c = obj.GetChild(i)
        if c.name == name:
            return c
    return None


try:
    wanted = {}
    for f in glob.glob(os.path.join(SPV_DIR, '*.ps.spv')):
        wanted[hashlib.sha1(open(f, 'rb').read()).hexdigest()] = os.path.basename(f)
    cap = rd.OpenCaptureFile()
    log('OpenFile: %s' % cap.OpenFile(PATH, '', None))
    sd = cap.GetStructuredData()
    modules = {}  # id do modulo -> nome do .ps.spv
    found = {name: [] for name in wanted.values()}
    total = unoptimized = 0
    for chunk in sd.chunks:
        if chunk.name == 'vkCreateShaderModule':
            info = child(chunk, 'CreateInfo')
            code = child(info, 'pCode') if info else None
            module = child(chunk, 'ShaderModule')
            if code is not None and module is not None:
                data = bytes(sd.buffers[code.data.basic.u])  # bloco de bytes: indice em sd.buffers
                dump = os.environ.get('RDOC_DUMP_DIR')
                if dump:  # todos os modulos do quadro, para comparar com o pacote
                    os.makedirs(dump, exist_ok=True)
                    open(os.path.join(dump, 'mod_%d.spv' % module.data.basic.u), 'wb').write(data)
                name = wanted.get(hashlib.sha1(data).hexdigest())
                if name:
                    modules[module.data.basic.u] = name
        elif chunk.name == 'vkCreateGraphicsPipelines':
            info = child(chunk, 'CreateInfo')
            if info is None:
                continue
            flags = child(info, 'flags')
            flag_value = flags.data.basic.u if flags is not None else 0
            total += 1
            unoptimized += flag_value & 1
            stages = child(info, 'pStages')
            names = []
            if stages is not None:
                for i in range(stages.NumChildren()):
                    m = child(stages.GetChild(i), 'module')
                    if m is not None and m.data.basic.u in modules:
                        names.append(modules[m.data.basic.u])
            for n in names:
                found[n].append(flag_value)
    log('pipelines na captura: %d, sem otimizacao: %d' % (total, unoptimized))
    log('modulos dos pares achados: %d de %d' % (len(modules), len(wanted)))
    for name, flags in sorted(found.items()):
        log('%s: %d pipelines, flags %s' % (name, len(flags), ' '.join('%x' % f for f in flags)))
except Exception:
    log(traceback.format_exc())
log('script terminou')
with open(OUT, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
