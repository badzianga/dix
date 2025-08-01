#include <stdio.h>
#include "chunk.h"
#include "debug.h"
#include "parser.h"
#include "value.h"
#include "vm.h"

static int simple_instruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int push1byte_instruction(const char* name, Chunk* chunk, int offset) {
    int8_t value = chunk->code[offset + 1];
    printf("%-8s %d\n", name, value);
    return offset + 2;
}

static int push2byte_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t high = chunk->code[offset + 1];
    uint8_t low = chunk->code[offset + 2];
    int16_t value = (int16_t)(high << 8 | low);
    printf("%-8s %d\n", name, value);
    return offset + 3;
}

static int const_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t index = chunk->code[offset + 1];
    printf("%-8s %d '", name, index);
    print_value(chunk->constant_pool.values[index]);
    printf("'\n");
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
        case OP_NOP:    return simple_instruction("nop", offset);
        case OP_B2I:    return simple_instruction("b2i", offset);
        case OP_B2F:    return simple_instruction("b2f", offset);
        case OP_I2B:    return simple_instruction("i2b", offset);
        case OP_I2F:    return simple_instruction("i2f", offset);
        case OP_F2B:    return simple_instruction("f2b", offset);
        case OP_F2I:    return simple_instruction("f2i", offset);
        case OP_BIPUSH: return push1byte_instruction("bipush", chunk, offset);
        case OP_SIPUSH: return push2byte_instruction("sipush", chunk, offset);
        case OP_LOADC:  return const_instruction("loadc", chunk, offset);
        case OP_IADD:   return simple_instruction("iadd", offset);
        case OP_FADD:   return simple_instruction("fadd", offset);
        case OP_ISUB:   return simple_instruction("isub", offset);
        case OP_FSUB:   return simple_instruction("fsub", offset);
        case OP_IMUL:   return simple_instruction("imul", offset);
        case OP_FMUL:   return simple_instruction("fmul", offset);
        case OP_IDIV:   return simple_instruction("idiv", offset);
        case OP_FDIV:   return simple_instruction("fdiv", offset);
        case OP_INEG:   return simple_instruction("ineg", offset);
        case OP_FNEG:   return simple_instruction("fneg", offset);
        case OP_TRUE:   return simple_instruction("true", offset);
        case OP_FALSE:  return simple_instruction("false", offset);
        case OP_NOT:    return simple_instruction("not", offset);
        case OP_PRINT:  return simple_instruction("print", offset);
        case OP_RETURN: return simple_instruction("return", offset);
        default: {
            printf("debug::disassemble_instruction: unknown opcode: %d\n", instruction);
            return offset + 1;
        }
    }
}

void print_tokens(TokenArray* token_array) {
    const Token* end = token_array->tokens + token_array->count;
    for (const Token* token = token_array->tokens; token != end; ++token) {
        TokenType type = token->type;

        if ((type >= TOKEN_IDENTIFIER && type <= TOKEN_STRING_LITERAL) || type == TOKEN_ERROR) {
            printf(
                "Line: %d,\ttoken: %s,\tvalue: %.*s\n",
                token->line,
                token_as_cstr(type),
                token->length,
                token->start
            );
        }
        else {
            printf(
                "Line: %d,\ttoken: %s\n",
                token->line,
                token_as_cstr(type)
            );
        }
    }
}

void print_ast(ASTNode* root, int indent) {
    for (int i = 0; i < indent; ++i) printf("  ");

    switch (root->type) {
        case AST_NODE_BINARY: {
            printf("Binary: %s\n", token_as_cstr(root->binary.op));
            print_ast(root->binary.left, indent + 1);
            print_ast(root->binary.right, indent + 1);
        } break;
        case AST_NODE_UNARY: {
            printf("Unary: %s\n", token_as_cstr(root->unary.op));
            print_ast(root->unary.right, indent + 1);
        } break;
        case AST_NODE_LITERAL: {
            printf("Literal: ");
            print_value(root->literal);
            printf("\n");
        } break;
        case AST_NODE_CAST: {
            char* value_type = "";
            switch (root->cast.target_type) {
                case VALUE_BOOL: value_type = "bool"; break;
                case VALUE_INT: value_type = "int"; break;
                case VALUE_FLOAT: value_type = "float"; break;
                default: break;
            }
            printf("Cast: %s\n", value_type);
            print_ast(root->cast.expression, indent + 1);
        } break;
        default: {
            printf("Unknown: %d\n", root->type);
        } break;
    }
}

void disassemble_chunk(Chunk* chunk) {
    int offset = 0;
    while (offset < chunk->count) {
        offset = disassemble_instruction(chunk, offset);
    }
}
