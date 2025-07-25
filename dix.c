#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "vm.h"

static void repl() {
    char line[1024];
    for (;;) {
        printf(">>> ");

        if (fgets(line, sizeof(line), stdin)) {
            printf("\n");

            interpret(line);
        }
    }
}

static void run_file(const char* file_path) {
    char* source = read_file(file_path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == RESULT_COMPILE_ERROR || result == RESULT_RUNTIME_ERROR) exit(1);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        repl();
    }
    else if (argc == 2) {
        run_file(argv[1]);
    }
    else {
        fprintf(stderr, "usage: %s <input.dix>\n", argv[0]);
        return 1;
    }

    return 0;
}
