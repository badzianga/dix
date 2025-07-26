#include <stdlib.h>
#include "chunk.h"
#include "memory.h"
#include "value.h"

void write_chunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, old_capacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void free_chunk(Chunk* chunk) {
    free(chunk->code);
    free(chunk->lines);

    free_value_array(&chunk->constant_pool);

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
}

int add_constant(Chunk* chunk, Value value) {
    int index = chunk->constant_pool.count;
    push_to_value_array(&chunk->constant_pool, value);
    return index;
}
