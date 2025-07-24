#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#define READ_BYTE() (*vm.ip++)

typedef int Value;

typedef struct VM {
    const uint8_t* program;
    int program_size;
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

void interpret(const uint8_t* program, int program_size) {
    vm.program = program;
    vm.program_size = program_size;
    vm.ip = program;
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
