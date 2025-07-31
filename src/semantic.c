#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "value.h"

typedef struct Analyzer {
    bool had_error;
} Analyzer;

static Analyzer analyzer = { 0 };

static ASTNode* make_node_cast(ValueType target_type, ASTNode* expression) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_CAST;
    node->cast.target_type = target_type;
    node->cast.expression = expression;
    return node;
}

void analyze_ast(ASTNode* root) {
    switch (root->type) {
        case AST_NODE_BINARY: {
            ASTNode* left = root->binary.left;
            ASTNode* right = root->binary.right;
            analyze(left);
            analyze(right);

            switch (root->binary.op) {
                case TOKEN_PLUS:
                case TOKEN_MINUS:
                case TOKEN_ASTERISK:
                case TOKEN_SLASH:
                    if (left->inferred_type == VALUE_INT && right->inferred_type == VALUE_INT)
                        root->inferred_type = VALUE_INT;
                    else if (left->inferred_type == VALUE_FLOAT && right->inferred_type == VALUE_FLOAT)
                        root->inferred_type = VALUE_FLOAT;
                    else {
                        fprintf(stderr, "invalid operands for binary operation\n");
                        analyzer.had_error = true;
                    }
                default: break;
            }
        } break;
        case AST_NODE_UNARY: {
            ASTNode* right = root->unary.right;
            analyze(right);

            switch (root->unary.op) {
                case TOKEN_BANG: {
                    if (right->inferred_type != VALUE_BOOL) {
                        fprintf(stderr, "invalid type for '!' operator\n");
                        analyzer.had_error = true;
                    }
                    root->inferred_type = VALUE_BOOL;
                } break;
                case TOKEN_MINUS: {
                    ValueType type = right->inferred_type;
                    if (type == VALUE_INT || type == VALUE_FLOAT) {
                        root->inferred_type = type;
                    }
                    else {
                        fprintf(stderr, "invalid type for '-' operator\n");
                        analyzer.had_error = true;
                    }
                } break;
                default: break;
            }
        } break;
        case AST_NODE_LITERAL: {
            root->inferred_type = root->literal.type;
        } break;
        case AST_NODE_CAST: {
            root->inferred_type = root->cast.target_type;
        } break;
        default: {
            fprintf(stderr, "unknown node type: %d\n", root->type);
            analyzer.had_error = true;
        } break;
    }
}

bool analyze(ASTNode* root) {
    analyzer.had_error = false;
    analyze_ast(root);
    return !analyzer.had_error;
}
