#include <stdbool.h>
#include "compiler.h"
#include "parser.h"
#include "value.h"
#include "vm.h"

typedef struct Compiler {
    Chunk* chunk;
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

    if (node->inferred_type == VALUE_INT) {
        switch (node->binary.op) {
            case TOKEN_PLUS:     emit_byte(OP_IADD); break;
            case TOKEN_MINUS:    emit_byte(OP_ISUB); break;
            case TOKEN_ASTERISK: emit_byte(OP_IMUL); break;
            case TOKEN_SLASH:    emit_byte(OP_IDIV); break;
            default: break;
        }
    }
    else if (node->inferred_type == VALUE_FLOAT) {
        switch (node->binary.op) {
            case TOKEN_PLUS:     emit_byte(OP_FADD); break;
            case TOKEN_MINUS:    emit_byte(OP_FSUB); break;
            case TOKEN_ASTERISK: emit_byte(OP_FMUL); break;
            case TOKEN_SLASH:    emit_byte(OP_FDIV); break;
            default: break;
        }
    }
    else {
        // invalid operands
    }
}

static void unary(ASTNode* node) {
    traverse_ast(node->unary.right);

    if (node->unary.op == TOKEN_MINUS) {
        if (node->inferred_type == VALUE_INT) emit_byte(OP_INEG);
        else if (node->inferred_type == VALUE_FLOAT) emit_byte(OP_FNEG);
        else return;  // invalid operand
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

static void cast(ASTNode* node) {
    traverse_ast(node->cast.expression);

    switch (node->cast.target_type) {
        case VALUE_BOOL: {
            switch (node->cast.expression->inferred_type) {
                case VALUE_BOOL: break;
                case VALUE_INT: emit_byte(OP_I2B); break;
                case VALUE_FLOAT: emit_byte(OP_F2B); break;
                default: break;
            }
        } break;
        case VALUE_INT: {
            switch (node->cast.expression->inferred_type) {
                case VALUE_BOOL: emit_byte(OP_B2I); break;
                case VALUE_INT: break;
                case VALUE_FLOAT: emit_byte(OP_F2I); break;
                default: break;
            }
        } break;
        case VALUE_FLOAT: {
            switch (node->cast.expression->inferred_type) {
                case VALUE_BOOL: emit_byte(OP_B2F); break;
                case VALUE_INT: emit_byte(OP_I2F); break;
                case VALUE_FLOAT: break;
                default: break;
            }
        } break;
        default: break;
    }
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
        case AST_NODE_CAST: {
            cast(node); 
        } break;
        default: break;
    }
}

bool compile(ASTNode* ast, Chunk* chunk) {
    compiler.chunk = chunk;

    traverse_ast(ast);

    emit_bytes(OP_PRINT, OP_RETURN);

    return true;
}
