#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "value.h"
#include "vm.h"

#define VALUE_TYPE_STACK_MAX_SIZE 256

typedef struct Parser {
    TokenArray* tokens;
    Token* current;
    Chunk* current_chunk;
    bool had_error;
    bool panic_mode;

    ValueType stack[256];
    ValueType* stack_top;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFunc)();

typedef struct {
    ParseFunc prefix;
    ParseFunc infix;
    Precedence precedence;
} ParseRule;

static Parser parser = { 0 };

static void push(ValueType type) {
    *parser.stack_top++ = type;
}

static ValueType pop() {
    return *--parser.stack_top;
}

static void error_at(Token* token, const char* message);
static void error(const char* message);

static Token* current() {
    return parser.current;
}

static Token* previous() {
    return parser.current - 1;
}

static void advance() {
    for (;;) {
        ++parser.current;

        if (current()->type != TOKEN_ERROR) break;
        error_at(current(), current()->start);
    }
}

static void error_at(Token* token, const char* message) {
    if (parser.panic_mode) return;
    parser.panic_mode = true;

    fprintf(stderr, "[line %d] error", token->line);

    if (token->type == TOKEN_EOF) {
      fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR) {

    }
    else {
      fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error(const char* message) {
    error_at(previous(), message);
}

static void consume(TokenType type, const char* message) {
    if (current()->type == type) {
        advance();
        return;
    }
    error_at(current(), message);
}

static void emit_byte(uint8_t byte) {
    write_chunk(parser.current_chunk, byte, previous()->line);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

static uint8_t push_constant(Value value) {
    int index = add_constant(parser.current_chunk, value);
    if (index > UINT8_MAX) {
        error("for now, only 256 constants are supported in the constant pool");
        return 0;
    }
    return (uint8_t)index;
}

static void expression();
static void binary();
static void unary();
static void grouping();
static void integer();
static void floating();
static void literal();

static ParseRule rules[] = {
    [TOKEN_EOF]            = {NULL,     NULL,   PREC_NONE},

    [TOKEN_LEFT_PAREN]     = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]     = {NULL,     NULL,   PREC_NONE}, 
    [TOKEN_RIGHT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACKET]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACKET]  = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SEMICOLON]      = {NULL,     NULL,   PREC_NONE},

    [TOKEN_COLON]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COLON_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BANG]           = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER_EQUAL]  = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS_EQUAL]     = {NULL,     NULL,   PREC_NONE},

    [TOKEN_PLUS]           = {NULL,     binary, PREC_TERM},
    [TOKEN_PLUS_EQUAL]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]          = {unary,    binary, PREC_TERM},
    [TOKEN_MINUS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ASTERISK]       = {NULL,     binary, PREC_FACTOR},
    [TOKEN_ASTERISK_EQUAL] = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_SLASH_EQUAL]    = {NULL,     NULL,   PREC_NONE},

    [TOKEN_IDENTIFIER]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_INT_LITERAL]    = {integer,  NULL,   PREC_NONE},
    [TOKEN_FLOAT_LITERAL]  = {floating, NULL,   PREC_NONE},
    [TOKEN_STRING_LITERAL] = {NULL,     NULL,   PREC_NONE},

    [TOKEN_AND]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BOOL]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CONST]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_FLOAT]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FOR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUNC]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]             = {NULL,     NULL,   PREC_NONE},
    [TOKEN_INT]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NULL]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_OR]             = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]           = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]          = {NULL,     NULL,   PREC_NONE},

    [TOKEN_ERROR]          = {NULL,     NULL,   PREC_NONE},
};

static ParseRule* get_rule(TokenType type) {
    return &rules[type];
}

static void parse_precedence(Precedence precedence) {
    advance();
    ParseFunc prefix_rule = get_rule(previous()->type)->prefix;
    if (prefix_rule == NULL) {
        error("expected expression");
        return;
    }

    prefix_rule();
    while (precedence <= get_rule(current()->type)->precedence) {
        advance();
        ParseFunc infix_rule = get_rule(previous()->type)->infix;
        infix_rule();
    }
}    

static void expression() {
    parse_precedence(PREC_ASSIGNMENT);
}

static void binary() {
    Token* operator = previous();
    ParseRule* rule = get_rule(operator->type);
    parse_precedence((Precedence)(rule->precedence + 1));

    ValueType type2 = pop();
    ValueType type1 = pop();

    if (type1 == VALUE_INT && type2 == VALUE_INT) {
        switch (operator->type) {
            case TOKEN_PLUS:     emit_byte(OP_IADD); break;
            case TOKEN_MINUS:    emit_byte(OP_ISUB); break;
            case TOKEN_ASTERISK: emit_byte(OP_IMUL); break;
            case TOKEN_SLASH:    emit_byte(OP_IDIV); break;
            default: break;
        }
        push(VALUE_INT);
    }
    else if (type1 == VALUE_FLOAT && type2 == VALUE_FLOAT) {
        switch (operator->type) {
            case TOKEN_PLUS:     emit_byte(OP_FADD); break;
            case TOKEN_MINUS:    emit_byte(OP_FSUB); break;
            case TOKEN_ASTERISK: emit_byte(OP_FMUL); break;
            case TOKEN_SLASH:    emit_byte(OP_FDIV); break;
            default: break;
        }
        push(VALUE_FLOAT);
    }
    else {
        error_at(operator, "expected values of the same type");
        push(VALUE_NONE);
    }
}

static void unary() {
    Token* operator = previous();

    parse_precedence(PREC_UNARY);

    ValueType type = pop();

    if (operator->type == TOKEN_MINUS) {
        if (type == VALUE_INT) {
            emit_byte(OP_INEG);
            push(VALUE_INT);
        }
        else if (type == VALUE_FLOAT) {
            emit_byte(OP_FNEG);
            push(VALUE_FLOAT);
        }
        else {
            error_at(operator, "invalid value type for negation");
            push(VALUE_NONE);
        }
    }
    else if (operator->type == TOKEN_BANG) {
        if (type == VALUE_BOOL) {
            emit_byte(OP_NOT);
            push(VALUE_BOOL);
        }
        else {
            error_at(operator, "invalid value type for not operator");
            push(VALUE_NONE);
        }
    }
    else { // unreachable
        push(VALUE_NONE);
    }
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "expected ')' after expression");
}

static void integer() {
    int64_t value = strtoll(previous()->start, NULL, 10);
    if (value >= INT8_MIN && value <= INT8_MAX) {
        emit_bytes(OP_BIPUSH, (int8_t)value);
    }
    else if (value >= INT16_MIN && value <= INT16_MAX) {
        emit_byte(OP_SIPUSH);
        uint8_t high = (uint8_t)((value >> 8) & 0xFF);
        uint8_t low = (uint8_t)(value & 0xFF);
        emit_bytes(high, low);
    }
    else if (value >= INT32_MIN && value <= INT32_MAX) {
        emit_bytes(OP_LOADC, push_constant(INT_VALUE(value)));
    } else {
        error("64 bit integers are not supported for now");
    }
    push(VALUE_INT);
}

static void floating() {
    float value = strtof(previous()->start, NULL);
    emit_bytes(OP_LOADC, push_constant(FLOAT_VALUE(value)));
    push(VALUE_FLOAT);
}

static void literal() {
    switch (previous()->type) {
        case TOKEN_FALSE: {
            emit_byte(OP_FALSE);
            push(VALUE_BOOL);
        } break;
        case TOKEN_TRUE: {
            emit_byte(OP_TRUE);
            push(VALUE_BOOL);
        } break;
        default: break;
    }
}

bool compile(TokenArray* tokens, Chunk* chunk) {
    parser.tokens = tokens;
    parser.current = tokens->tokens;
    parser.current_chunk = chunk;
    parser.had_error = false;
    parser.panic_mode = false;

    parser.stack_top = parser.stack;

    expression();
    consume(TOKEN_EOF, "expected end of expression");
    emit_bytes(OP_PRINT, OP_RETURN);

    return !parser.had_error;
}
