#pragma once
#include <stdint.h>
#include "chunk.h"

#define VM_STACK_CAPACITY 256

// TODO: true/false values were pushed on the stack using BIPUSH as 1/0.
// With this, the information about being a boolean was discarded.
// That's why special OpCodes for true/false and comparisons are present.

typedef enum OpCode {
    OP_NOP = 0,
    OP_BIPUSH,
    OP_SIPUSH,
    OP_LOADC,
    OP_IADD,
    OP_FADD,
    OP_ISUB,
    OP_FSUB,
    OP_IMUL,
    OP_FMUL,
    OP_IDIV,
    OP_FDIV,
    OP_INEG,
    OP_FNEG,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_PRINT,
    OP_RETURN,
} OpCode;

typedef enum InterpretResult {
    RESULT_OK,
    RESULT_PARSE_ERROR,
    RESULT_COMPILE_ERROR,
    RESULT_RUNTIME_ERROR,
} InterpretResult;

InterpretResult interpret(const char* source);
