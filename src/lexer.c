#include <assert.h>
#include <ctype.h>
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

static Token read_number() {
    while (isdigit(peek())) advance();
    
    if (peek() == '.') {
        advance();

        while (isdigit(peek())) advance();
        if (peek() == 'f') advance();

        return make_token(TOKEN_FLOAT_LITERAL);
    }
    
    return make_token(TOKEN_INT_LITERAL);
}

static Token read_string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') ++lexer.line;
        advance();
    }

    if (is_at_end()) return make_error_token("unterminated string");

    advance();
    return make_token(TOKEN_STRING_LITERAL);
}

static Token read_keyword_or_identifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }

    static const char* keywords[] = {
        "and",
        "bool",
        "class",
        "const",
        "else",
        "false",
        "float",
        "for",
        "func",
        "if",
        "int",
        "null",
        "or",
        "print",
        "return",
        "this",
        "true",
        "var",
        "while",
    };
    const int keywords_amount = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < keywords_amount; ++i) {
        int length = (int)(lexer.current - lexer.start);
        int keyword_length = strlen(keywords[i]);

        if (length != keyword_length) continue;

        bool might_be_keyword = true;
        for (int j = 0; j < length; ++j) {
            if (*(lexer.start + j) != keywords[i][j]) {
                might_be_keyword = false;
                break;
            }
        }
        if (might_be_keyword) {
            return make_token(TOKEN_AND + i);
        }
    }
    return make_token(TOKEN_IDENTIFIER);
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
        case ':': return match('=') ? make_token(TOKEN_COLON_EQUAL) : make_token(TOKEN_COLON);
        case '=': return match('=') ? make_token(TOKEN_EQUAL_EQUAL) : make_token(TOKEN_EQUAL);
        case '!': return match('=') ? make_token(TOKEN_BANG_EQUAL) : make_token(TOKEN_BANG);
        case '>': return match('=') ? make_token(TOKEN_GREATER_EQUAL) : make_token(TOKEN_GREATER);
        case '<': return match('=') ? make_token(TOKEN_LESS_EQUAL) : make_token(TOKEN_LESS);
        case '+': return match('=') ? make_token(TOKEN_PLUS_EQUAL) : make_token(TOKEN_PLUS);
        case '-': return match('=') ? make_token(TOKEN_MINUS_EQUAL) : make_token(TOKEN_MINUS);
        case '*': return match('=') ? make_token(TOKEN_ASTERISK_EQUAL) : make_token(TOKEN_ASTERISK);
        case '/': return match('=') ? make_token(TOKEN_SLASH_EQUAL) : make_token(TOKEN_SLASH);
        case '"': return read_string();
        default: break;
    }

    if (isdigit(c)) {
        return read_number();
    }
    else if (isalpha(c) || c == '_') {
        return read_keyword_or_identifier();
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

const char* token_as_cstr(TokenType type) {
    static const char* token_strings[] = {
        "EOF",

        "(", ")", "{", "}", "[", "]",
        ",", ".", ";",

        ":", ":=", "=", "==", "!", "!=",
        ">", ">=", "<", "<=",

        "+", "+=", "-", "-=",
        "*", "*=", "/", "/=",

        "IDENTIFIER", "INT_LITERAL", "FLOAT_LITERAL", "STRING_LITERAL",

        "and", "bool", "class", "const", "else",
        "false", "float", "for", "func", "if",
        "int", "null", "or", "print", "return",
        "this", "true", "var", "while",

        "ERROR",
    };

    static_assert(
        sizeof(token_strings) / sizeof(token_strings[0]) == TOKEN_ERROR + 1,
        "lexer::token_as_cstr: not all token types are handled"
    );

    return token_strings[type];
}
