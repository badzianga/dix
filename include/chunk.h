#pragma once
#include <stdint.h>

typedef struct Chunk {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
} Chunk;

void write_chunk(Chunk* chunk, uint8_t byte, int line);
void free_chunk(Chunk* chunk);
