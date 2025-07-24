#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "lexer.h"

int main() {
    char* source = read_file("snippets/expression.dix");
    printf("%s\n", source);

    TokenArray tokens = lex(source);
    print_tokens(&tokens);

    free_tokens(&tokens);
    free(source);

    return 0;
}
