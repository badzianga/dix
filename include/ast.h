#pragma once
#include "lexer.h"
#include "value.h"

typedef enum ASTNodeType {
    AST_NODE_BINARY,
    AST_NODE_UNARY,
    AST_NODE_LITERAL,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;

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
    };
} ASTNode;

ASTNode* make_node_binary(ASTNode* left, TokenType op, ASTNode* right);
ASTNode* make_node_unary(TokenType op, ASTNode* right);
ASTNode* make_node_literal(Value value);

void print_ast(ASTNode* root, int indent);
void free_ast(ASTNode* root);
