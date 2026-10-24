# Roda dentro do qrenderdoc (--python): abre o PORT (build\lego_dimensions.exe) e dispara capturas nos
# instantes de RDOC_TIMES (segundos desde o inicio, separados por virgula). RDOC_ARGS = argumentos do jogo.
import os
import time
import traceback

import renderdoc as rd

ROOT = r'C:\Jogos\lego-dimensions-pc'
OUT = os.path.join(ROOT, 'build', 'rdoc_port')
LOG = os.path.join(OUT, 'rdoc_capture.log')
EXE = os.path.join(ROOT, 'build', 'lego_dimensions.exe')
ARGS = os.environ.get('RDOC_ARGS', '--no-keyboard --no-sdl-pad --fps60')
TIMES = sorted(float(t) for t in os.environ.get('RDOC_TIMES', '60').split(',') if t)
# RDOC_TRIGGER=<arquivo>: captura cada vez que esse arquivo aparece (um script de teste o cria na hora certa) e o
# apaga; RDOC_END = tempo maximo em segundos
TRIGGER = os.environ.get('RDOC_TRIGGER', '')
END = float(os.environ.get('RDOC_END', (TIMES[-1] if TIMES else 0) + 25))


def log(text):
    with open(LOG, 'a', encoding='utf-8') as f:
        f.write('%s %s\n' % (time.strftime('%H:%M:%S'), text))


try:
    os.makedirs(OUT, exist_ok=True)
    open(LOG, 'w').close()
    opts = rd.CaptureOptions()
    opts.apiValidation = False
    opts.allowFullscreen = False
    result = rd.ExecuteAndInject(EXE, ROOT, ARGS, [], os.path.join(OUT, 'port'), opts, False)
    log('ExecuteAndInject: ident %s' % result.ident)
    target = None
    for attempt in range(40):
        target = rd.CreateTargetControl('', result.ident, 'claude', True)
        if target is not None and target.Connected():
            break
        target = None
        time.sleep(0.5)
    if target is None:
        raise RuntimeError('nao conectou ao processo')
    log('conectado: pid %s' % target.GetPID())
    start = time.time()
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
        if TRIGGER and os.path.exists(TRIGGER):
            os.remove(TRIGGER)
            log('disparando captura pelo gatilho em t=%.1f' % (time.time() - start))
            target.TriggerCapture(1)
    log('fim do laco em t=%.1f' % (time.time() - start))
    target.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
