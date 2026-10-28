"""Region-aware discovery for owner-supplied PS4 Dimensions dumps.

CUSA00935 support is adapted from JonyGOi commit 0aeb8b9. Automatic discovery
never selects an update or DLC folder belonging to the other region.
"""
import os
import struct

SUPPORTED_TITLE_IDS = ('CUSA01176', 'CUSA00935')


def read_sfo(path):
    with open(path, 'rb') as stream:
        data = stream.read(1024 * 1024 + 1)
    if not 20 <= len(data) <= 1024 * 1024 or data[:4] != b'\0PSF':
        raise ValueError('invalid PS4 metadata signature or size')
    version, keys, values, count = struct.unpack_from('<4I', data, 4)
    if version != 0x101 or not 1 <= count <= 512 or not 20 + 16 * count <= keys <= values <= len(data):
        raise ValueError('invalid PS4 metadata tables')
    out = {}
    for i in range(count):
        key_off, fmt, length, capacity, value_off = struct.unpack_from('<HHIII', data, 20 + 16 * i)
        at = keys + key_off
        stop = data.find(b'\0', at, values)
        if not keys <= at < values or stop < at or length > capacity or values + value_off + capacity > len(data):
            raise ValueError('PS4 metadata field exceeds tables')
        key = data[at:stop].decode('ascii')
        if key in out:
            raise ValueError('duplicate PS4 metadata key')
        raw = data[values + value_off:values + value_off + length]
        if fmt == 0x404:
            if length != 4:
                raise ValueError('invalid PS4 integer metadata')
            out[key] = struct.unpack('<I', raw)[0]
        elif fmt in (0x204, 0x004):
            out[key] = raw.rstrip(b'\0').decode('utf-8')
    return out


def title_id(app0):
    try:
        value = read_sfo(os.path.join(app0, 'sce_sys', 'param.sfo')).get('TITLE_ID', '')
        return value if value in SUPPORTED_TITLE_IDS else ''
    except (OSError, ValueError, UnicodeError, struct.error):
        return ''


def find_patch(app0):
    app0 = os.path.normpath(app0)
    tid = title_id(app0)
    if not tid:
        return ''
    name, parent = os.path.basename(app0), os.path.dirname(app0)
    for candidate in (os.path.join(app0, name + '-patch'), os.path.join(parent, name + '-patch'),
                      os.path.join(app0, tid + '-patch'), os.path.join(parent, tid + '-patch')):
        if title_id(candidate) == tid:
            return candidate
    return ''


def default_addcont(app0):
    tid = title_id(app0)
    appdata = os.environ.get('APPDATA', '')
    candidate = os.path.join(appdata, 'shadPS4', 'addcont', tid)
    return candidate if appdata and tid and os.path.isdir(candidate) else ''
