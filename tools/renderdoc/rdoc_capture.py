# Roda dentro do qrenderdoc (--python): abre o shadPS4 com o jogo e dispara uma captura cada vez
# que aparece um arquivo gatilho_N em OUT (criado de fora, quando a cena certa esta na tela).
import glob
import os
import time
import traceback

import renderdoc as rd

OUT = r'C:\Jogos\lego-dimensions-pc\build\rdoc'
LOG = os.path.join(OUT, 'rdoc_capture.log')
EXE = r'C:\src\shadPS4\build\shadps4.exe'
GAME = r'C:\Jogos\CUSA01176\eboot.bin'
END = float(os.environ.get('RDOC_END', '420'))


def log(text):
    with open(LOG, 'a', encoding='utf-8') as f:
        f.write('%s %s\n' % (time.strftime('%H:%M:%S'), text))


try:
    os.makedirs(OUT, exist_ok=True)
    open(LOG, 'w').close()
    for old in glob.glob(os.path.join(OUT, 'gatilho_*')) + glob.glob(os.path.join(OUT, 'fim')):
        os.remove(old)
    opts = rd.CaptureOptions()
    opts.apiValidation = False
    opts.allowFullscreen = False
    result = rd.ExecuteAndInject(EXE, os.path.dirname(EXE), '"%s"' % GAME, [], os.path.join(OUT, 'ref'), opts, False)
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
    done = set()
    while time.time() - start < END and target.Connected() and not os.path.exists(os.path.join(OUT, 'fim')):
        msg = target.ReceiveMessage(None)
        if msg.type == rd.TargetControlMessageType.NewCapture:
            cap = msg.newCapture
            log('captura %s: %s (frame %s) t=%.1f' % (cap.captureId, cap.path, cap.frameNumber, time.time() - start))
        elif msg.type == rd.TargetControlMessageType.Disconnected:
            log('desconectado')
            break
        for trigger in glob.glob(os.path.join(OUT, 'gatilho_*')):
            if trigger not in done:
                done.add(trigger)
                log('disparando captura (%s) em t=%.1f' % (os.path.basename(trigger), time.time() - start))
                target.TriggerCapture(1)
    log('fim do laco em t=%.1f' % (time.time() - start))
    target.Shutdown()
except Exception:
    log('ERRO\n' + traceback.format_exc())
log('script terminou')
