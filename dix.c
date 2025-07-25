#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "vm.h"

int main(int argc, char** argv) {
    const char* executable = argv[0];
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input.dix>\n", argv[0]);
        exit(1);
    }

    const char* input_file = argv[1];
    char* source = read_file(input_file);

    interpret(source);

    return 0;
}
