#include <stdbool.h>
#include <stdio.h>
#include "chunk.h"
#include "compiler.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "lexer.h"
#include "value.h"
#include "vm.h"

#define READ_BYTE() (*vm.ip++)
#define IBINARY_OP(op) \
    do { \
        int b = AS_INT(pop()); \
        int a = AS_INT(pop()); \
        push(INT_VALUE(a op b)); \
    } while (false)

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

static InterpretResult run() {
    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_NOP: {
                continue;
            } break;
            case OP_BIPUSH: {
                push(INT_VALUE((int8_t)READ_BYTE()));
            } break;
            case OP_SIPUSH: {
                uint8_t high = READ_BYTE();
                uint8_t low = READ_BYTE();
                int16_t value = (int16_t)(high << 8 | low);
                push(INT_VALUE(value));
            } break;
            case OP_IADD: {
                IBINARY_OP(+);
            } break;
            case OP_ISUB: {
                IBINARY_OP(-);
            } break;
            case OP_IMUL: {
                IBINARY_OP(*);
            } break;
            case OP_IDIV: {
                IBINARY_OP(/);
            } break;
            case OP_INEG: {
                push(INT_VALUE(-AS_INT(pop())));
            } break;
            case OP_TRUE: {
                push(BOOL_VALUE(true));
            } break;
            case OP_FALSE: {
                push(BOOL_VALUE(false));
            } break;
            case OP_NOT: {
                push(BOOL_VALUE(!AS_BOOL(pop())));
            } break;
            case OP_PRINT: {
                print_value(pop());
            } break;
            case OP_RETURN: {
                return RESULT_OK;
            } break;
            default: {
                fprintf(stderr, "vm::interpret: unknown instruction %d\n", instruction);
                return RESULT_RUNTIME_ERROR;
            }
        }
    }
}

InterpretResult interpret(const char* source) {
    TokenArray tokens = lex(source);
    Chunk chunk = { 0 };

#ifdef DEBUG
    print_tokens(&tokens);
#endif

    if (!compile(&tokens, &chunk)) {
        free_chunk(&chunk);
        free_tokens(&tokens);
        return RESULT_COMPILE_ERROR;
    }

#ifdef DEBUG
    disassemble_chunk(&chunk);
#endif

    vm.chunk = &chunk;
    vm.ip = chunk.code;
    vm.stack_top = vm.stack;

    InterpretResult result = run();
    free_chunk(&chunk);
    free_tokens(&tokens);
    return result;
}
