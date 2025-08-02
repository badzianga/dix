#pragma once
#include "lexer.h"
#include "value.h"

typedef enum ASTNodeType {
    AST_NODE_BINARY,
    AST_NODE_UNARY,
    AST_NODE_LITERAL,
    AST_NODE_CAST,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    ValueType inferred_type;
    int line;

    union {
        struct {
            struct ASTNode* left;
            TokenType op;
            struct ASTNode* right;
        } binary;

        struct {
            TokenType op;
            struct ASTNode* right;
        } unary;

        Value literal;

        struct {
            ValueType target_type;
            struct ASTNode* expression;
        } cast;
    };
} ASTNode;

bool parse(TokenArray* token_array, ASTNode** output);

void free_ast(ASTNode* root);
