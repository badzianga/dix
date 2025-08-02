#include <stdlib.h>
#include "make_node.h"

ASTNode* make_node_binary(int line, ASTNode* left, TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_BINARY;
    node->inferred_type = VALUE_NONE;
    node->line = line;
    node->binary.left = left;
    node->binary.op = op;
    node->binary.right = right;
    return node;
}

ASTNode* make_node_unary(int line, TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_UNARY;
    node->inferred_type = VALUE_NONE;
    node->line = line;
    node->unary.op = op;
    node->unary.right = right;
    return node;
}

ASTNode* make_node_literal(int line, Value value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_LITERAL;
    node->inferred_type = VALUE_NONE;
    node->line = line;
    node->literal = value;
    return node;
}

ASTNode* make_node_cast(int line, ValueType target_type, ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_CAST;
    node->inferred_type = VALUE_NONE;
    node->line = line;
    node->cast.target_type = target_type;
    node->cast.expression = expression;
    return node;
}
