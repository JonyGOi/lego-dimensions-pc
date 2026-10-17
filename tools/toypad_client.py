# Cliente de teste que fala o protocolo do app LegoToypad com o listener do port.
import socket
import struct
import sys
import time

PORT = 9191


def send(payload, expect=0):
    with socket.create_connection(('127.0.0.1', PORT), timeout=3) as s:
        s.sendall(payload)
        s.shutdown(socket.SHUT_WR)
        data = b''
        while len(data) < expect:
            chunk = s.recv(expect - len(data))
            if not chunk:
                break
            data += chunk
        return data


def vehicle_tag(uid, vehicle_id):
    tag = bytearray(180)
    tag[0:3] = uid[0:3]
    tag[3] = 0x88 ^ uid[0] ^ uid[1] ^ uid[2]
    tag[4:8] = uid[3:7]
    tag[8] = uid[3] ^ uid[4] ^ uid[5] ^ uid[6]
    tag[9] = 0x48
    tag[12:15] = bytes([0xE1, 0x10, 0x12])
    tag[0x24 * 4:0x24 * 4 + 4] = struct.pack('<I', vehicle_id)
    tag[0x26 * 4 + 1] = 1
    return bytes(tag)


def load(pad, index, tag, path=''):
    p = path.encode('utf-8')
    send(bytes([1, pad, index, 0, 0]) + tag + struct.pack('<H', len(p)) + p)


step = sys.argv[1]
if step == 'load':
    load(1, 0, vehicle_tag(bytes([4, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66]), 1000))
    load(2, 1, vehicle_tag(bytes([4, 0x21, 0x32, 0x43, 0x54, 0x65, 0x76]), 1001))
elif step == 'move':
    send(bytes([3, 3, 4, 2, 1]))
elif step == 'remove':
    send(bytes([2, 1, 0, 0, 0]))
elif step == 'led':
    r = send(bytes([4, 0, 0, 0, 0]), 40)  # Fase 60: formato v2 ('L', contador, 2, 3, 12 bytes por zona)
    print('LED contador %d versao %d' % (r[1], r[2]))
    for z in range(3):
        b = r[4 + 12 * z:16 + 12 * z]
        print('  zona %d modo %d cor %02x%02x%02x origem %02x%02x%02x aceso %d apagado %d vezes %d velocidade %d' % tuple(b))
