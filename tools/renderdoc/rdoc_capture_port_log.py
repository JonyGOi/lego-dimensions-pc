# Roda dentro do qrenderdoc (--python): como rdoc_capture_port.py, mas abre o port por um cmd.exe com a saida
# redirecionada para build\rdoc_port\jogo.log (o RenderDoc entra no processo filho). RDOC_ARGS, RDOC_TIMES.
import os
import time
import traceback

import renderdoc as rd

ROOT = os.environ.get('RDOC_ROOT', r'C:\Jogos\lego-dimensions-pc')  # (__file__ nao existe no qrenderdoc --python)
OUT = os.path.join(ROOT, 'build', 'rdoc_port')
LOG = os.path.join(OUT, 'rdoc_capture.log')
EXE = os.path.join(ROOT, 'build', 'lego_dimensions.exe')
ARGS = os.environ.get('RDOC_ARGS', '--no-keyboard --no-sdl-pad --fps60')
TIMES = sorted(float(t) for t in os.environ.get('RDOC_TIMES', '60').split(','))
END = TIMES[-1] + 25


def log(text):
    with open(LOG, 'a', encoding='utf-8') as f:
        f.write('%s %s\n' % (time.strftime('%H:%M:%S'), text))


def connect(ident):
    for _ in range(60):
        target = rd.CreateTargetControl('', ident, 'claude', True)
        if target is not None and target.Connected():
            return target
        time.sleep(0.5)
    return None


try:
    os.makedirs(OUT, exist_ok=True)
    open(LOG, 'w').close()
    opts = rd.CaptureOptions()
    opts.apiValidation = False
    opts.allowFullscreen = False
    opts.hookIntoChildren = True
    cmd = os.path.join(os.environ['SystemRoot'], 'System32', 'cmd.exe')
    line = '/c ""%s" %s > "%s" 2>&1"' % (EXE, ARGS, os.path.join(OUT, 'jogo.log'))
    result = rd.ExecuteAndInject(cmd, ROOT, line, [], os.path.join(OUT, 'port'), opts, False)
    log('ExecuteAndInject (cmd): ident %s' % result.ident)
    parent = connect(result.ident)
    if parent is None:
        raise RuntimeError('nao conectou ao cmd')
    target = None
    start = time.time()
    while target is None and time.time() - start < 60:
        msg = parent.ReceiveMessage(None)
        if msg.type == rd.TargetControlMessageType.NewChild:
            log('filho: pid %s ident %s' % (msg.newChild.processId, msg.newChild.ident))
            target = connect(msg.newChild.ident)
        time.sleep(0.1)
    parent.Shutdown()
    if target is None:
        raise RuntimeError('nao conectou ao jogo')
    log('conectado ao jogo: pid %s' % target.GetPID())
    pending = list(TIMES)
    while time.time() - start < END and target.Connected():
        msg = target.ReceiveMessage(None)
        if msg.type == rd.TargetControlMessageType.NewCapture:
            cap = msg.newCapture
            log('captura %s: %s (frame %s) t=%.1f' % (cap.captureId, cap.path, cap.frameNumber, time.time() - start))
        elif msg.type == rd.TargetControlMessageType.Disconnected:
            log('desconectado')
            break
        if pending and time.time() - start >= pending[0]:
            log('disparando captura em t=%.1f' % (time.time() - start))
            pending.pop(0)
            target.TriggerCapture(1)
    log('fim do laco em t=%.1f' % (time.time() - start))
    target.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
