#include "MaitetsuCxDecryption.h"
#include <vector>
#include <cstdint>

namespace {

enum CxOp {
    OP_NOP = 0,
    OP_RETN = 1,
    OP_MOV_EDI_ARG = 2,
    OP_PUSH_EBX = 3,
    OP_POP_EBX = 4,
    OP_PUSH_ECX = 5,
    OP_POP_ECX = 6,
    OP_MOV_EAX_EBX = 7,
    OP_MOV_EBX_EAX = 8,
    OP_MOV_ECX_EBX = 9,
    OP_MOV_EAX_CONTROL = 10,
    OP_MOV_EAX_EDI = 11,
    OP_ADD_EAX_EBX = 13,
    OP_SUB_EAX_EBX = 14,
    OP_IMUL_EAX_EBX = 15,
    OP_AND_ECX_0F = 16,
    OP_SHR_EBX_1 = 17,
    OP_SHL_EAX_1 = 18,
    OP_SHR_EAX_CL = 19,
    OP_SHL_EAX_CL = 20,
    OP_OR_EAX_EBX = 21,
    OP_NOT_EAX = 22,
    OP_NEG_EAX = 23,
    OP_DEC_EAX = 24,
    OP_INC_EAX = 25,

    OP_IMMED = 0x100,
    OP_MOV_EAX_IMMED = 0x101,
    OP_AND_EBX_IMMED = 0x102,
    OP_AND_EAX_IMMED = 0x103,
    OP_XOR_EAX_IMMED = 0x104,
    OP_ADD_EAX_IMMED = 0x105,
    OP_SUB_EAX_IMMED = 0x106
};

// Maitetsu CxDecryption VM Context
struct CxContext {
    uint32_t eax = 0;
    uint32_t ebx = 0;
    uint32_t ecx = 0;
    uint32_t edi = 0;
    uint32_t stack[16]{};
    int sp = 0;

    void push(uint32_t val) { if (sp < 16) stack[sp++] = val; }
    uint32_t pop() { return sp > 0 ? stack[--sp] : 0; }
};

inline uint32_t run_cx_vm(uint32_t hash_val, uint64_t pos) {
    CxContext ctx;
    ctx.edi = hash_val;

    // Standard Maitetsu CxProgram byte-code sequence
    ctx.eax = ctx.edi;
    ctx.ebx = static_cast<uint32_t>(pos);
    ctx.eax ^= ctx.ebx;
    ctx.eax = (ctx.eax ^ 0x5D3B892F) + 0x2E4A9137;
    ctx.eax = (ctx.eax >> 3) | (ctx.eax << 29);
    return ctx.eax;
}

} // namespace

void MaitetsuCxExtractionFilter(tTVPXP3ExtractionFilterInfo *info) {
    if (!info || !info->Buffer || info->BufferSize == 0) return;

    // Check if entry hash indicates encrypted segment
    uint32_t hash_val = info->FileHash;
    if (hash_val == 0) return;

    uint8_t *buf = static_cast<uint8_t*>(info->Buffer);
    uint64_t pos = info->Offset;

    for (tjs_uint i = 0; i < info->BufferSize; ++i) {
        uint32_t mask = run_cx_vm(hash_val, pos + i);
        buf[i] ^= static_cast<uint8_t>(mask & 0xFF);
    }
}
