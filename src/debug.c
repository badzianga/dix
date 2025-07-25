#include <stdio.h>
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

static int simple_instruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int push1byte_instruction(const char* name, Chunk* chunk, int offset) {
    int8_t value = chunk->code[offset + 1];
    printf("%-8s %d\n", name, value);
    return offset + 2;
}

static int push2byte_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t high = chunk->code[offset + 1];
    uint8_t low = chunk->code[offset + 2];
    int16_t value = (int16_t)(high << 8 | low);
    printf("%-8s %d\n", name, value);
    return offset + 3;
}

static int const_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t index = chunk->code[offset + 1];
    printf("%-8s %d '", name, index);
    print_value(chunk->constant_pool.values[index]);
    printf("'\n");
    return offset + 2;
}

static int disassemble_instruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_NOP:    return simple_instruction("nop", offset);
        case OP_BIPUSH: return push1byte_instruction("bipush", chunk, offset);
        case OP_SIPUSH: return push2byte_instruction("sipush", chunk, offset);
        case OP_LOADC:  return const_instruction("loadc", chunk, offset);
        case OP_IADD:   return simple_instruction("iadd", offset);
        case OP_FADD:   return simple_instruction("fadd", offset);
        case OP_ISUB:   return simple_instruction("isub", offset);
        case OP_FSUB:   return simple_instruction("fsub", offset);
        case OP_IMUL:   return simple_instruction("imul", offset);
        case OP_FMUL:   return simple_instruction("fmul", offset);
        case OP_IDIV:   return simple_instruction("idiv", offset);
        case OP_FDIV:   return simple_instruction("fdiv", offset);
        case OP_INEG:   return simple_instruction("ineg", offset);
        case OP_FNEG:   return simple_instruction("fneg", offset);
        case OP_TRUE:   return simple_instruction("true", offset);
        case OP_FALSE:  return simple_instruction("false", offset);
        case OP_NOT:    return simple_instruction("not", offset);
        case OP_PRINT:  return simple_instruction("print", offset);
        case OP_RETURN: return simple_instruction("return", offset);
        default: {
            printf("debug::disassemble_instruction: unknown opcode: %d\n", instruction);
            return offset + 1;
        }
    }
}

void disassemble_chunk(Chunk* chunk) {
    int offset = 0;
    while (offset < chunk->count) {
        offset = disassemble_instruction(chunk, offset);
    }
}
