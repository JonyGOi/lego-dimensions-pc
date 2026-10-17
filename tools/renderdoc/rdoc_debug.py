# Roda dentro do qrenderdoc (--python). Depura o shader de pixel do evento RDOC_EID no pixel RDOC_XY
# ("x,y") de RDOC_FILE e grava o valor final das variaveis de RDOC_VARS ("_221,_227,...").
import os
import traceback

import renderdoc as rd

PATH = os.environ['RDOC_FILE']
EID = int(os.environ['RDOC_EID'])
X, Y = [int(v) for v in os.environ['RDOC_XY'].split(',')]
WANT = set(os.environ.get('RDOC_VARS', '').split(','))
OUT = os.environ.get('RDOC_OUT', os.path.splitext(PATH)[0]) + '_debug%d.txt' % EID
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
    controller.SetFrameEvent(EID, True)
    inputs = rd.DebugPixelInputs()
    inputs.sample = 0
    inputs.primitive = rd.ReplayController.NoPreference
    trace = controller.DebugPixel(X, Y, inputs)
    log('debugger: %s' % (trace.debugger is not None))
    values = {}
    steps = 0
    while True:
        states = controller.ContinueDebug(trace.debugger)
        if not states:
            break
        for state in states:
            steps += 1
            for change in state.changes:
                var = change.after
                name = var.name
                try:
                    v = var.value.f32v
                    values[name] = [v[i] for i in range(max(1, var.columns))]
                except Exception:
                    pass
    log('%d passos, %d variaveis' % (steps, len(values)))
    for name in sorted(values, key=lambda n: (len(n), n)):
        if not WANT or name in WANT or WANT == {''}:
            log('%s = %s' % (name, ' '.join('%.6g' % f for f in values[name])))
    controller.FreeTrace(trace)
    flush()
    controller.Shutdown()
    cap.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
    flush()
flush()
os._exit(0)
