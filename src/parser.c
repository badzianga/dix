#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "value.h"

typedef struct Parser {
    TokenArray* tokens;
    Token* current;
    bool had_error;
    bool panic_mode;
} Parser;

static Parser parser = { 0 };

inline static Token* previous_token() {
    return parser.current - 1;
}

static bool match(int argc, ...) {
    TokenType token = parser.current->type;
    va_list argv;
    va_start(argv, argc);
    for (int i = 0; i < argc; ++i) {
        if (token == va_arg(argv, TokenType)) {
            va_end(argv);
            ++parser.current;
            return true;
        }
    }
    va_end(argv);
    return false;
}

static void consume_expected(TokenType token, const char* error_if_fail) {
    if (parser.current->type != token) {
        fprintf(stderr, "error: %s\n", error_if_fail);
        exit(1);
    }
    ++parser.current;
}

static ASTNode* make_node_binary(ASTNode* left, TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_BINARY;
    node->inferred_type = VALUE_NONE;
    node->binary.left = left;
    node->binary.op = op;
    node->binary.right = right;
    return node;
}

static ASTNode* make_node_unary(TokenType op, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_UNARY;
    node->inferred_type = VALUE_NONE;
    node->unary.op = op;
    node->unary.right = right;
    return node;
}

static ASTNode* make_node_literal(Value value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NODE_LITERAL;
    node->inferred_type = VALUE_NONE;
    node->literal = value;
    return node;
}

static ASTNode* parse_expression();
static ASTNode* parse_term();
static ASTNode* parse_factor();
static ASTNode* parse_unary();
static ASTNode* parse_primary();

static ASTNode* parse_expression() {
    return parse_term();
}

static ASTNode* parse_term() {
    ASTNode* left = parse_factor();
    while(match(2, TOKEN_PLUS, TOKEN_MINUS)) {
        TokenType op = previous_token()->type;
        ASTNode* right = parse_factor();
        left = make_node_binary(left, op, right);
    }
    return left;
}

static ASTNode* parse_factor() {
    ASTNode* left = parse_unary();
    while (match(2, TOKEN_ASTERISK, TOKEN_SLASH)) {
        TokenType op = previous_token()->type;
        ASTNode* right = parse_unary();
        left = make_node_binary(left, op, right);
    }
    return left;
}

static ASTNode* parse_unary() {
    if (match(2, TOKEN_MINUS, TOKEN_BANG)) {
        TokenType op = previous_token()->type;
        ASTNode* right = parse_primary();
        return make_node_unary(op, right);
    }
    return parse_primary();
}

static ASTNode* parse_primary() {
    if (match(1, TOKEN_INT_LITERAL)) {
        int32_t value = strtol(previous_token()->start, NULL, 10);
        return make_node_literal(INT_VALUE(value));
    }
    if (match(1, TOKEN_FLOAT_LITERAL)) {
        float value = strtof(previous_token()->start, NULL);
        return make_node_literal(FLOAT_VALUE(value));
    }
    if (match(1, TOKEN_TRUE)) {
        return make_node_literal(BOOL_VALUE(true));
    }
    if (match(1, TOKEN_FALSE)) {
        return make_node_literal(BOOL_VALUE(false));
    }
    if (match(1, TOKEN_LEFT_PAREN)) {
        ASTNode* inside = parse_expression();
        consume_expected(TOKEN_RIGHT_PAREN, "expected closing parenthesis");
        return inside;
    }

    if (parser.current->type == TOKEN_ERROR) {
        fprintf(stderr, "error: %.*s\n", parser.current->length, parser.current->start);
    }
    else {
        fprintf(stderr, "error: unexpected value: '%.*s'\n", parser.current->length, parser.current->start);
    }
    exit(1);
}

bool parse(TokenArray* token_array, ASTNode** output) {
    parser.tokens = token_array;
    parser.current = token_array->tokens;

    *output = parse_expression();

    return !parser.had_error;
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
    free(root);
}
