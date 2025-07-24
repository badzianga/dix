#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "vm.h"

#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(op) do { Value b = pop(); Value a = pop(); push(a op b); } while (false)

typedef int Value;

typedef struct VM {
    Chunk* chunk;
    const uint8_t* ip;

    Value stack[VM_STACK_CAPACITY];
    Value* stack_top;
} VM;

static VM vm = { 0 };

static void push(Value value) {
    *vm.stack_top++ = value;
}

static Value pop() {
    return *--vm.stack_top;
}

void interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = chunk->code;
    vm.stack_top = vm.stack;

    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_NOP: {
                continue;
            } break;
            case OP_BIPUSH: {
                push((int8_t)READ_BYTE());
            } break;
            case OP_IADD: {
                BINARY_OP(+);
            } break;
            case OP_ISUB: {
                BINARY_OP(-);
            } break;
            case OP_IMUL: {
                BINARY_OP(*);
            } break;
            case OP_IDIV: {
                BINARY_OP(/);
            } break;
            case OP_INEG: {
                push(-pop());
            } break;
            case OP_PRINT: {
                printf("%d\n", pop());
            } break;
            case OP_RETURN: {
                return;
            } break;
            default: {
                fprintf(stderr, "vm::interpret: unknown instruction %d\n", instruction);
                exit(1);
            }
        }
    }
}
