#pragma once
#include <stdint.h>
#include "chunk.h"

#define VM_STACK_CAPACITY 256

typedef enum OpCode {
    OP_NOP = 0,
    OP_BIPUSH,
    OP_IADD,
    OP_ISUB,
    OP_IMUL,
    OP_IDIV,
    OP_INEG,
    OP_PRINT,
    OP_RETURN,
} OpCode;

void interpret(Chunk* chunk);
