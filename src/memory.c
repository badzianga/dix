#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

void* reallocate(void* pointer, size_t old_size, size_t new_size) {
    (void) old_size;
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, new_size);
    if (result == NULL) {
        fprintf(stderr, "memory::reallocate: failed to realloc memory\n");
        exit(1);
    }
    return result;
}
