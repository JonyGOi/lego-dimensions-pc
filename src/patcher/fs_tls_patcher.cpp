#include "patcher/fs_tls_patcher.h"

#include <cstring>

#include <Zydis/Zydis.h>

namespace patcher {

FsPatchStats PatchFsThreadPointer(u8* code, u64 size, u32 gs_offset) {
    FsPatchStats stats;
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    // O segmento R-X do eboot tambem contem .rodata, entao uma varredura linear dessincroniza.
    // Em vez disso, todo byte 0x64 (prefixo FS) e testado como inicio de instrucao, e so a
    // forma exata abaixo e alterada - a chance de dados aleatorios baterem com ela e desprezivel.
    const u8* end = code + size;
    u8* p = code;
    while ((p = static_cast<u8*>(std::memchr(p, 0x64, static_cast<size_t>(end - p)))) != nullptr) {
        ZydisDecodedInstruction insn;
        ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];
        if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, p, static_cast<ZyanUSize>(end - p), &insn,
                                               ops))) {
            ++p;
            continue;
        }
        const ZydisDecodedOperand* mem = nullptr;
        for (u8 i = 0; i < insn.operand_count_visible; ++i) {
            if (ops[i].type == ZYDIS_OPERAND_TYPE_MEMORY && ops[i].mem.segment == ZYDIS_REGISTER_FS) {
                mem = &ops[i];
            }
        }
        if (!mem || !(insn.attributes & ZYDIS_ATTRIB_HAS_SEGMENT_FS)) {
            ++p;
            continue;
        }
        ++stats.candidates;

        const bool is_tp_load = insn.mnemonic == ZYDIS_MNEMONIC_MOV &&
                                ops[0].type == ZYDIS_OPERAND_TYPE_REGISTER && ops[0].size == 64 &&
                                mem == &ops[1] && mem->mem.base == ZYDIS_REGISTER_NONE &&
                                mem->mem.index == ZYDIS_REGISTER_NONE &&
                                mem->mem.disp.size != 0 && mem->mem.disp.value == 0 &&
                                (insn.raw.disp.size == 32 || insn.raw.disp.size == 64);
        if (!is_tp_load) {
            const bool abs_load = insn.mnemonic == ZYDIS_MNEMONIC_MOV && insn.length == 9 &&
                                  ops[0].type == ZYDIS_OPERAND_TYPE_REGISTER && ops[0].size == 64 &&
                                  mem == &ops[1] && mem->mem.base == ZYDIS_REGISTER_NONE &&
                                  mem->mem.index == ZYDIS_REGISTER_NONE && insn.raw.disp.size == 32;
            if (abs_load) {
                ++stats.absolute_nonzero;
                if (stats.absolute_nonzero_samples.size() < 8) {
                    stats.absolute_nonzero_samples.emplace_back(static_cast<u64>(p - code),
                                                                mem->mem.disp.value);
                }
            } else {
                ++stats.other_forms;
            }
            ++p;
            continue;
        }

        p[0] = 0x65; // FS -> GS
        if (insn.raw.disp.size == 32) {
            std::memcpy(p + insn.raw.disp.offset, &gs_offset, sizeof(u32));
        } else {
            const u64 wide = gs_offset;
            std::memcpy(p + insn.raw.disp.offset, &wide, sizeof(u64));
        }
        ++stats.patched;
        if (stats.patched_samples.size() < 8) {
            stats.patched_samples.push_back(static_cast<u64>(p - code));
        }
        p += insn.length;
    }
    return stats;
}

} // namespace patcher
