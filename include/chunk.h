#pragma once
#include <stdint.h>
#include "value.h"

typedef struct Chunk {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ValueArray constant_pool;
} Chunk;

void write_chunk(Chunk* chunk, uint8_t byte, int line);
void free_chunk(Chunk* chunk);

int add_constant(Chunk* chunk, Value value);
