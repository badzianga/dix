#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input.dix>\n", argv[0]);
        exit(1);
    }

    uint8_t program[] = { OP_BIPUSH, 100, OP_PRINT, OP_RETURN };
    int program_size = sizeof(program) / sizeof(uint8_t);

    interpret(program, program_size);

    return 0;
}
