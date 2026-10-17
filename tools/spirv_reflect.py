#!/usr/bin/env python3
"""Lista a interface de um modulo SPIR-V: entradas e saidas (location), blocos de uniformes e
storage buffers, imagens e samplers (set / binding) e o bloco de push constants com o tamanho.

Serve para saber o que um shader recompilado do jogo (assets/shaders_spv) exige de um pipeline
do Vulkan antes de tentar liga-lo.

uso: python tools/spirv_reflect.py arquivo.spv [outro.spv ...]
"""
import struct
import sys

STORAGE = {0: "UniformConstant", 1: "Input", 2: "Uniform", 3: "Output", 9: "PushConstant", 12: "StorageBuffer"}
DECOR = {2: "Block", 3: "BufferBlock", 11: "BuiltIn", 30: "Location", 33: "Binding", 34: "DescriptorSet", 35: "Offset", 6: "ArrayStride"}


def reflect(path):
    data = open(path, "rb").read()
    words = struct.unpack(f"<{len(data) // 4}I", data)
    types, names, decor, member_offsets, variables, constants = {}, {}, {}, {}, [], {}
    i = 5
    while i < len(words):
        count, op = words[i] >> 16, words[i] & 0xFFFF
        a = words[i + 1:i + count]
        if op == 5:  # OpName
            names[a[0]] = b"".join(struct.pack("<I", w) for w in a[1:]).split(b"\0")[0].decode()
        elif op == 71:  # OpDecorate
            decor.setdefault(a[0], {})[DECOR.get(a[1], a[1])] = a[2] if len(a) > 2 else True
        elif op == 72 and a[2] == 35:  # OpMemberDecorate Offset
            member_offsets.setdefault(a[0], {})[a[1]] = a[3]
        elif op == 22:
            types[a[0]] = ("float", a[1])
        elif op == 21:
            types[a[0]] = ("int" if a[2] else "uint", a[1])
        elif op == 23:
            types[a[0]] = ("vec", a[1], a[2])
        elif op == 24:
            types[a[0]] = ("mat", a[1], a[2])
        elif op == 25:
            types[a[0]] = ("image", a[2], a[3], a[4], a[6])  # dim, depth, arrayed, sampled
        elif op == 26:
            types[a[0]] = ("sampler",)
        elif op == 27:
            types[a[0]] = ("sampled_image", a[1])
        elif op == 28:
            types[a[0]] = ("array", a[1], a[2])
        elif op == 29:
            types[a[0]] = ("runtime_array", a[1])
        elif op == 30:
            types[a[0]] = ("struct", list(a[1:]))
        elif op == 32:
            types[a[0]] = ("pointer", a[1], a[2])
        elif op == 43:
            constants[a[1]] = a[2]
        elif op == 59:
            variables.append((a[1], a[0], a[2]))
        i += count

    def size(t):
        kind = types.get(t, ("?",))
        if kind[0] in ("float", "int", "uint"):
            return kind[1] // 8
        if kind[0] == "vec":
            return size(kind[1]) * kind[2]
        if kind[0] == "mat":
            return size(kind[1]) * kind[2]
        if kind[0] == "array":
            return size(kind[1]) * constants.get(kind[2], 0)
        if kind[0] == "struct":
            offsets = member_offsets.get(t, {})
            return max((offsets.get(n, 0) + size(m) for n, m in enumerate(kind[1])), default=0)
        return 0

    def describe(t):
        kind = types.get(t, ("?",))
        if kind[0] in ("float", "int", "uint"):
            return f"{kind[0]}{kind[1]}"
        if kind[0] == "vec":
            return f"{describe(kind[1])}x{kind[2]}"
        if kind[0] == "mat":
            return f"mat{kind[2]}"
        if kind[0] == "image":
            dims = {0: "1D", 1: "2D", 2: "3D", 3: "Cube", 5: "Buffer"}.get(kind[1], kind[1])
            return f"image{dims}{' array' if kind[3] else ''}{' depth' if kind[2] == 1 else ''}{' (storage)' if kind[4] == 2 else ''}"
        if kind[0] == "sampler":
            return "sampler"
        if kind[0] == "sampled_image":
            return "sampled " + describe(kind[1])
        if kind[0] == "array":
            return f"{describe(kind[1])}[{constants.get(kind[2], '?')}]"
        if kind[0] == "runtime_array":
            return f"{describe(kind[1])}[]"
        if kind[0] == "struct":
            return "struct{" + ", ".join(describe(m) for m in kind[1]) + "}"
        return "?"

    print(f"== {path} ({len(data)} bytes)")
    for var, pointer, storage in sorted(variables, key=lambda v: (v[2], decor.get(v[0], {}).get("Location", decor.get(v[0], {}).get("Binding", 0)))):
        target = types[pointer][2]
        d = decor.get(var, {})
        kind = STORAGE.get(storage, storage)
        where = ""
        if "Location" in d:
            where = f"location {d['Location']}"
        if "Binding" in d:
            where = f"set {d.get('DescriptorSet', 0)} binding {d['Binding']}"
        if "BuiltIn" in d:
            where = f"builtin {d['BuiltIn']}"
        extra = ""
        if kind == "PushConstant" or (types.get(target, ("",))[0] == "struct" and kind in ("Uniform", "StorageBuffer")):
            block = decor.get(target, {})
            extra = f"  tamanho {size(target)} bytes{'  (BufferBlock)' if 'BufferBlock' in block else ''}"
        if storage in (7,):
            continue
        print(f"  {kind:16} {where:22} {names.get(var, ''):28} {describe(target)[:90]}{extra}")


for name in sys.argv[1:]:
    reflect(name)
