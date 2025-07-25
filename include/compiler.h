#pragma once
#include <stdbool.h>
#include "chunk.h"
#include "lexer.h"

bool compile(TokenArray* tokens, Chunk* chunk);
