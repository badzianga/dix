#include <stdio.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static int simple_instruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int push_instruction(const char* name, Chunk* chunk, int offset) {
    int8_t value = chunk->code[offset + 1];
    printf("%-8s %d\n", name, value);
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
        case OP_NOP: return simple_instruction("nop", offset);
        case OP_BIPUSH: return push_instruction("bipush", chunk, offset);
        case OP_PRINT: return simple_instruction("print", offset);
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
