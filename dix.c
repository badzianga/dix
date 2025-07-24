#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input.dix>\n", argv[0]);
        exit(1);
    }

    Chunk chunk = { 0 };

    write_chunk(&chunk, OP_BIPUSH, 1);
    write_chunk(&chunk, 10, 1);
    write_chunk(&chunk, OP_INEG, 1);
    write_chunk(&chunk, OP_BIPUSH, 1);
    write_chunk(&chunk, 20, 1);
    write_chunk(&chunk, OP_IADD, 1);
    write_chunk(&chunk, OP_PRINT, 1);
    write_chunk(&chunk, OP_RETURN, 1);

    disassemble_chunk(&chunk);

    interpret(&chunk);

    free_chunk(&chunk);

    return 0;
}
