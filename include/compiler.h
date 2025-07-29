#pragma once
#include <stdbool.h>
#include "chunk.h"
#include "parser.h"

bool compile(ASTNode* ast, Chunk* chunk);
