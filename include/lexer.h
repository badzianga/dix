#pragma once

typedef enum {
    TOKEN_EOF,             // EOF

    TOKEN_LEFT_PAREN,      // (
    TOKEN_RIGHT_PAREN,     // )
    TOKEN_LEFT_BRACE,      // {
    TOKEN_RIGHT_BRACE,     // }
    TOKEN_LEFT_BRACKET,    // [
    TOKEN_RIGHT_BRACKET,   // ]
    TOKEN_COMMA,           // ,
    TOKEN_DOT,             // .
    TOKEN_SEMICOLON,       // ;
    TOKEN_COLON,           // :

    TOKEN_EQUAL,           // =
    TOKEN_EQUAL_EQUAL,     // ==
    TOKEN_BANG,            // !
    TOKEN_BANG_EQUAL,      // !=
    TOKEN_GREATER,         // >
    TOKEN_GREATER_EQUAL,   // >=
    TOKEN_LESS,            // <
    TOKEN_LESS_EQUAL,      // <=

    TOKEN_PLUS,            // +
    TOKEN_PLUS_EQUAL,      // +=
    TOKEN_MINUS,           // -
    TOKEN_MINUS_EQUAL,     // -=
    TOKEN_ASTERISK,        // *
    TOKEN_ASTERISK_EQUAL,  // *=
    TOKEN_SLASH,           // /
    TOKEN_SLASH_EQUAL,     // /=

    TOKEN_IDENTIFIER,      // x
    TOKEN_INT_LITERAL,     // 0 
    TOKEN_FLOAT_LITERAL,   // 0.0
    TOKEN_STRING_LITERAL,  // ""

    TOKEN_AND,             // and
    TOKEN_CLASS,           // class
    TOKEN_CONST,           // const
    TOKEN_ELSE,            // else
    TOKEN_FALSE,           // false
    TOKEN_FOR,             // for
    TOKEN_FUNC,            // func
    TOKEN_IF,              // if
    TOKEN_NULLL,           // null
    TOKEN_OR,              // or
    TOKEN_PRINT,           // print
    TOKEN_RETURN,          // return
    TOKEN_THIS,            // this
    TOKEN_TRUE,            // true
    TOKEN_VAR,             // var
    TOKEN_WHILE,           // while

    TOKEN_ERROR,           // ERROR
} TokenType;
