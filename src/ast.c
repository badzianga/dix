#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "value.h"

ASTNode* make_node_binary(ASTNode* left, TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_BINARY;
    node->binary.left = left;
    node->binary.op = op;
    node->binary.right = right;
    return node;
}

ASTNode* make_node_unary(TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_UNARY;
    node->unary.op = op;
    node->unary.right = right;
    return node;
}

ASTNode* make_node_literal(Value value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_LITERAL;
    node->literal = value;
    return node;
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
        default: {
            fprintf(stderr, "Unknown AST node type: %d\n", root->type);
            exit(1);
        } break;
    }
}

void free_ast(ASTNode* root) {
    switch (root->type) {
        case AST_NODE_BINARY: {
            free(root->binary.left);
            free(root->binary.right);
        } break;
        case AST_NODE_UNARY: {
            free(root->unary.right);
        } break;
        case AST_NODE_LITERAL: break;
        default: {
            fprintf(stderr, "Unknown AST node type: %d\n", root->type);
            exit(1);
        }
    }
}
