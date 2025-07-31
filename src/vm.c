#include <stdbool.h>
#include <stdio.h>
#include "chunk.h"
#include "compiler.h"
#ifdef DEBUG
#include "debug.h"
#endif
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "value.h"
#include "vm.h"

#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(type, AS_type, out_VALUE, op) \
    do { \
        type b = AS_type(pop()); \
        type a = AS_type(pop()); \
        push(out_VALUE(a op b)); \
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
            case OP_LOADC: {
                uint8_t index = READ_BYTE();
                push(vm.chunk->constant_pool.values[index]);
            } break;
            case OP_IADD: {
                BINARY_OP(int, AS_INT, INT_VALUE, +);
            } break;
            case OP_FADD: {
                BINARY_OP(float, AS_FLOAT, FLOAT_VALUE, +);
            } break;
            case OP_ISUB: {
                BINARY_OP(int, AS_INT, INT_VALUE, -);
            } break;
            case OP_FSUB: {
                BINARY_OP(float, AS_FLOAT, FLOAT_VALUE, -);
            } break;
            case OP_IMUL: {
                BINARY_OP(int, AS_INT, INT_VALUE, *);
            } break;
            case OP_FMUL: {
                BINARY_OP(float, AS_FLOAT, FLOAT_VALUE, *);
            } break;
            case OP_IDIV: {
                BINARY_OP(int, AS_INT, INT_VALUE, /);
            } break;
            case OP_FDIV: {
                BINARY_OP(float, AS_FLOAT, FLOAT_VALUE, /);
            } break;
            case OP_INEG: {
                push(INT_VALUE(-AS_INT(pop())));
            } break;
            case OP_FNEG: {
                push(FLOAT_VALUE(-AS_FLOAT(pop())));
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
                putchar('\n');
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
#ifdef DEBUG
    print_tokens(&tokens);
    printf("----------------------------------------------------------------\n");
#endif

    ASTNode* ast = NULL;
    if (!parse(&tokens, &ast)) {
        free_ast(ast);
        free_tokens(&tokens);
        return RESULT_PARSE_ERROR;
    }
#ifdef DEBUG
    print_ast(ast, 0);
    printf("----------------------------------------------------------------\n");
#endif

    if (!analyze(ast)) {
        free_ast(ast);
        free_tokens(&tokens);
        return RESULT_ANALYZE_ERROR;
    }
#ifdef DEBUG
    print_ast(ast, 0);
    printf("----------------------------------------------------------------\n");
#endif

    Chunk chunk = { 0 };
    if (!compile(ast, &chunk)) {
        free_chunk(&chunk);
        free_ast(ast);
        free_tokens(&tokens);
        return RESULT_COMPILE_ERROR;
    }
#ifdef DEBUG
    disassemble_chunk(&chunk);
    printf("----------------------------------------------------------------\n");
#endif

    vm.chunk = &chunk;
    vm.ip = chunk.code;
    vm.stack_top = vm.stack;

    InterpretResult result = run();
    free_chunk(&chunk);
    free_ast(ast);
    free_tokens(&tokens);
    return result;
}
