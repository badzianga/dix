#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int main() {
    char* source = read_file("test.dix");

    printf("%s\n", source);

    free(source);
    return 0;
}
