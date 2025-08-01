#pragma once
#include "chunk.h"
#include "lexer.h"
#include "parser.h"

void print_tokens(TokenArray* token_array);
void print_ast(ASTNode* root, int indent);
void disassemble_chunk(Chunk* chunk);
