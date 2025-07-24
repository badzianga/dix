#pragma once
#include <stdint.h>

#define VM_STACK_CAPACITY 256

typedef enum OpCode {
    OP_NOP = 0,
    OP_BIPUSH,
    OP_PRINT,
    OP_RETURN,
} OpCode;

void interpret(const uint8_t* program, int program_size);
