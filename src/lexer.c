#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "memory.h"
#include "stdbool.h"

typedef struct Lexer {
    const char* start;
    const char* current;
    int line;
} Lexer;

static Lexer lexer;

static bool is_at_end() {
    return *lexer.current == '\0';
}

static char peek() {
    return *lexer.current;
}

static char advance() {
    return *lexer.current++;
}

static bool match(char expected) {
    if (is_at_end()) return false;
    if (*lexer.current != expected) return false;
    ++lexer.current;
    return true;
}

static Token make_token(TokenType type) {
    return (Token){
        .type = type,
        .start = lexer.start,
        .line = lexer.line,
        .length = (int)(lexer.current - lexer.start)
    };
}

static Token make_error_token(const char* message) {
    return (Token) {
        .type = TOKEN_ERROR,
        .start = message,
        .line = lexer.line,
        .length = strlen(message)
    };
}

static void skip_whitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                ++lexer.line;
                advance();
                break;
            default:
                return;
        }
    }
}

static Token next_token() {
    skip_whitespace();
    lexer.start = lexer.current;

    if (is_at_end()) {
        return make_token(TOKEN_EOF);
    }

    char c = advance();

    switch (c) {
        case '(': return make_token(TOKEN_LEFT_PAREN);
        case ')': return make_token(TOKEN_RIGHT_PAREN);
        case '{': return make_token(TOKEN_LEFT_BRACE);
        case '}': return make_token(TOKEN_RIGHT_BRACE);
        case '[': return make_token(TOKEN_LEFT_BRACKET);
        case ']': return make_token(TOKEN_RIGHT_BRACKET);
        case ',': return make_token(TOKEN_COMMA);
        case '.': return make_token(TOKEN_DOT);
        case ';': return make_token(TOKEN_SEMICOLON);
        case ':': return make_token(TOKEN_COLON);
        case '=': return match('=') ? make_token(TOKEN_EQUAL_EQUAL) : make_token(TOKEN_EQUAL);
        case '!': return match('=') ? make_token(TOKEN_BANG_EQUAL) : make_token(TOKEN_BANG);
        case '>': return match('=') ? make_token(TOKEN_GREATER_EQUAL) : make_token(TOKEN_GREATER);
        case '<': return match('=') ? make_token(TOKEN_LESS_EQUAL) : make_token(TOKEN_LESS);
        case '+': return match('=') ? make_token(TOKEN_PLUS_EQUAL) : make_token(TOKEN_PLUS);
        case '-': return match('=') ? make_token(TOKEN_MINUS_EQUAL) : make_token(TOKEN_MINUS);
        case '*': return match('=') ? make_token(TOKEN_ASTERISK_EQUAL) : make_token(TOKEN_ASTERISK);
        case '/': return match('=') ? make_token(TOKEN_SLASH_EQUAL) : make_token(TOKEN_SLASH);
        default: break;
    }

    return make_error_token("unexpected character");
}

TokenArray lex(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;

    TokenArray array = { 0 };

    for (;;) {
        if (array.capacity < array.count + 1) {
            int old_capacity = array.capacity;
            array.capacity = GROW_CAPACITY(old_capacity);
            array.tokens = GROW_ARRAY(Token, array.tokens, old_capacity, array.capacity);
        }

        Token token = next_token();
        array.tokens[array.count++] = token;

        if (token.type == TOKEN_EOF) break;
    }

    return array;
}

void free_tokens(TokenArray* token_array) {
    free(token_array->tokens);
    token_array->tokens = NULL;
    token_array->count = 0;
    token_array->capacity = 0;
}

void print_tokens(TokenArray* token_array) {
    const Token* end = token_array->tokens + token_array->count;
    for (const Token* token = token_array->tokens; token != end; ++token) {
        TokenType type = token->type;

        if (type == TOKEN_ERROR) {
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

const char* token_as_cstr(TokenType type) {
    static const char* token_strings[] = {
        "EOF",

        "(", ")", "{", "}", "[", "]",
        ",", ".", ";", ":",

        "=", "==", "!", "!=",
        ">", ">=", "<", "<=",

        "+", "+=", "-", "-=",
        "*", "*=", "/", "/=",

        "IDENTIFIER", "INT", "FLOAT", "STRING",

        "and", "class", "const", "else",
        "false", "for", "func", "if",
        "null", "or", "print", "return",
        "this", "true", "var", "while",

        "ERROR",
    };

    static_assert(
        sizeof(token_strings) / sizeof(token_strings[0]) == TOKEN_ERROR + 1,
        "lexer::token_as_cstr: not all token types are handled"
    );

    return token_strings[type];
}
