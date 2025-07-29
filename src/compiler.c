#include <stdbool.h>
#include "compiler.h"
#include "parser.h"
#include "value.h"
#include "vm.h"

typedef struct Compiler {
    Chunk* chunk;
    bool had_error;
} Compiler;

static Compiler compiler = { 0 };

static void traverse_ast(ASTNode* node);

static void emit_byte(uint8_t byte) {
    write_chunk(compiler.chunk, byte, 0);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

static uint8_t push_constant(Value value) {
    int index = add_constant(compiler.chunk, value);
    if (index > UINT8_MAX) {
        return 0;
    }
    return (uint8_t)index;
}

static void binary(ASTNode* node) {
    traverse_ast(node->binary.left);
    traverse_ast(node->binary.right);

    switch (node->binary.op) {
        case TOKEN_PLUS:     emit_byte(OP_IADD); break;
        case TOKEN_MINUS:    emit_byte(OP_ISUB); break;
        case TOKEN_ASTERISK: emit_byte(OP_IMUL); break;
        case TOKEN_SLASH:    emit_byte(OP_IDIV); break;
        default: break;
    }
}

static void unary(ASTNode* node) {
    traverse_ast(node->unary.right);

    if (node->unary.op == TOKEN_MINUS) {
        emit_byte(OP_INEG);
    }
    else if (node->unary.op == TOKEN_BANG) {
        emit_byte(OP_NOT);
    }
}

static void boolean(ASTNode* node) {
    if (node->literal.as.bool_ == true) {
        emit_byte(OP_TRUE);
    }
    else {
        emit_byte(OP_FALSE);
    }
}

static void integer(ASTNode* node) {
    int32_t value = node->literal.as.int_;
    if (value >= INT8_MIN && value <= INT8_MAX) {
        emit_bytes(OP_BIPUSH, (int8_t)value);
    }
    else if (value >= INT16_MIN && value <= INT16_MAX) {
        emit_byte(OP_SIPUSH);
        uint8_t high = (uint8_t)((value >> 8) & 0xFF);
        uint8_t low = (uint8_t)(value & 0xFF);
        emit_bytes(high, low);
    }
    else if (value >= INT32_MIN && value <= INT32_MAX) {
        emit_bytes(OP_LOADC, push_constant(INT_VALUE(value)));
    }
}

static void floating(ASTNode* node) {
    emit_bytes(OP_LOADC, push_constant(FLOAT_VALUE(node->literal.as.float_)));
}

void traverse_ast(ASTNode* node) {
    switch (node->type) {
        case AST_NODE_BINARY: {
            binary(node);
        } break;
        case AST_NODE_UNARY: {
            unary(node);
        } break;
        case AST_NODE_LITERAL: {
            switch (node->literal.type) {
                case VALUE_BOOL: {
                    boolean(node);
                } break;
                case VALUE_INT: {
                    integer(node);
                } break;
                case VALUE_FLOAT: {
                    floating(node);
                } break;
                default: break;
            }
        } break;
        default: break;
    }
}

bool compile(ASTNode* ast, Chunk* chunk) {
    compiler.chunk = chunk;
    compiler.had_error = false;

    traverse_ast(ast);

    emit_bytes(OP_PRINT, OP_RETURN);

    return !compiler.had_error;
}
