#include <stdlib.h>
#include "make_node.h"

ASTNode* make_node_binary(ASTNode* left, TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_BINARY;
    node->inferred_type = VALUE_NONE;
    node->binary.left = left;
    node->binary.op = op;
    node->binary.right = right;
    return node;
}

ASTNode* make_node_unary(TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_UNARY;
    node->inferred_type = VALUE_NONE;
    node->unary.op = op;
    node->unary.right = right;
    return node;
}

ASTNode* make_node_literal(Value value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_LITERAL;
    node->inferred_type = VALUE_NONE;
    node->literal = value;
    return node;
}

ASTNode* make_node_cast(ValueType target_type, ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_CAST;
    node->inferred_type = VALUE_NONE;
    node->cast.target_type = target_type;
    node->cast.expression = expression;
    return node;
}
