"""Compressao XPRESS (Huffman) da API de compressao do Windows (cabinet.dll), modo cru: o jogo
descompacta com a mesma API (Decompress), sabendo o tamanho original pelo indice."""
import ctypes
from ctypes import wintypes

COMPRESS_ALGORITHM_XPRESS_HUFF = 4
COMPRESS_RAW = 1 << 29

_cab = ctypes.WinDLL('cabinet.dll')
_cab.CreateCompressor.argtypes = [wintypes.DWORD, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p)]
_cab.Compress.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_size_t,
                          ctypes.POINTER(ctypes.c_size_t)]
_cab.CreateDecompressor.argtypes = _cab.CreateCompressor.argtypes
_cab.Decompress.argtypes = _cab.Compress.argtypes


class Packer:
    def __init__(self):
        self.handle = ctypes.c_void_p()
        if not _cab.CreateCompressor(COMPRESS_ALGORITHM_XPRESS_HUFF | COMPRESS_RAW, None, ctypes.byref(self.handle)):
            raise OSError('CreateCompressor falhou')
        self.unpacker = ctypes.c_void_p()
        if not _cab.CreateDecompressor(COMPRESS_ALGORITHM_XPRESS_HUFF | COMPRESS_RAW, None, ctypes.byref(self.unpacker)):
            raise OSError('CreateDecompressor falhou')

    def pack(self, data):
        cap = len(data) + len(data) // 2 + 1024
        out = ctypes.create_string_buffer(cap)
        size = ctypes.c_size_t()
        if not _cab.Compress(self.handle, data, len(data), out, cap, ctypes.byref(size)):
            raise OSError('Compress falhou')
        return out.raw[:size.value]

    def unpack(self, data, size):
        out = ctypes.create_string_buffer(size)
        got = ctypes.c_size_t()
        if not _cab.Decompress(self.unpacker, data, len(data), out, size, ctypes.byref(got)) or got.value != size:
            raise OSError('Decompress falhou')
        return out.raw
