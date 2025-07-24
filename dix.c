#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "lexer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input.dix>\n", argv[0]);
        exit(1);
    }

    char* source = read_file(argv[1]);
    printf("%s\n", source);

    TokenArray tokens = lex(source);
    print_tokens(&tokens);

    free_tokens(&tokens);
    free(source);

    return 0;
}
