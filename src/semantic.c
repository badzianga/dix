#include <stdio.h>
#include "lexer.h"
#include "make_node.h"
#include "parser.h"
#include "semantic.h"
#include "value.h"

typedef struct Analyzer {
    bool had_error;
    bool panic_mode;
} Analyzer;

static Analyzer analyzer = { 0 };

static void error(ASTNode* node, const char* message) {
    if (analyzer.panic_mode) return;
    analyzer.had_error = true;
    analyzer.panic_mode = true;
    fprintf(stderr, "[line %d] error: %s\n", node->line, message);
}

static void analyze_ast(ASTNode* root) {
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
                    else if (left->inferred_type == VALUE_INT && right->inferred_type == VALUE_FLOAT) {
                        root->binary.left = make_node_cast(left->line, VALUE_FLOAT, left);
                        root->inferred_type = VALUE_FLOAT;
                    }
                    else if (left->inferred_type == VALUE_FLOAT && right->inferred_type == VALUE_INT) {
                        root->binary.right = make_node_cast(right->line, VALUE_FLOAT, right);
                        root->inferred_type = VALUE_FLOAT;
                    }
                    else {
                        error(root, "incompatible types for binary operation");
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
                        error(root, "incompatible type for '!' operator");
                    }
                    root->inferred_type = VALUE_BOOL;
                } break;
                case TOKEN_MINUS: {
                    ValueType type = right->inferred_type;
                    if (type == VALUE_INT || type == VALUE_FLOAT) {
                        root->inferred_type = type;
                    }
                    else {
                        error(root, "incompatible type for '-' operator");
                    }
                } break;
                default: break;
            }
        } break;
        case AST_NODE_LITERAL: {
            root->inferred_type = root->literal.type;
        } break;
        case AST_NODE_CAST: {
            analyze(root->cast.expression);
            root->inferred_type = root->cast.target_type;
        } break;
        default: break;
    }
}

bool analyze(ASTNode* root) {
    analyzer.had_error = false;
    analyze_ast(root);
    return !analyzer.had_error;
}
