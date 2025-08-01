#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "make_node.h"
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

inline static Token* next_token() {
    return parser.current + 1;
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

static void error_at(Token* token, const char* message) {
    if (parser.panic_mode) return;
    parser.panic_mode = true;
    fprintf(stderr, "[line %d] error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR) {}
    else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error_at_current(const char* message) {
    error_at(parser.current, message);
}

static void consume_expected(TokenType token, const char* error_if_fail) {
    if (parser.current->type != token) {
        error_at_current(error_if_fail);
        return;
    }
    ++parser.current;
}

static ASTNode* parse_expression();
static ASTNode* parse_term();
static ASTNode* parse_factor();
static ASTNode* parse_unary();
static ASTNode* parse_cast();
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
        ASTNode* right = parse_cast();
        return make_node_unary(op, right);
    }
    return parse_cast();
}

static ASTNode* parse_cast() {
    if (parser.current->type == TOKEN_LEFT_PAREN && next_token()->type) {
        parser.current += 2;
        TokenType type = previous_token()->type;
        ValueType value_type = VALUE_NONE;
        switch (type) {
            case TOKEN_BOOL: value_type = VALUE_BOOL; break;
            case TOKEN_INT: value_type = VALUE_INT; break;
            case TOKEN_FLOAT: value_type = VALUE_FLOAT; break;
            default: break;
        }
        consume_expected(TOKEN_RIGHT_PAREN, "expected closing parenthesis after cast");
        ASTNode* expression = parse_cast();
        return make_node_cast(value_type, expression);
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
        error_at_current(parser.current->start);
    }
    else {
        error_at_current("unexpected value");
    }
    return NULL;
}

bool parse(TokenArray* token_array, ASTNode** output) {
    parser.tokens = token_array;
    parser.current = token_array->tokens;
    parser.had_error = false;
    parser.panic_mode = false;

    *output = parse_expression();

    return !parser.had_error;
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
        case AST_NODE_CAST: {
            free(root->cast.expression);
        } break;
        default: {
            fprintf(stderr, "unknown AST node type: %d\n", root->type);
        }
    }
    free(root);
}
