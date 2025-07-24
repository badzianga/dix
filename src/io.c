#include <stdio.h>
#include <stdlib.h>
#include "io.h"

char* read_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "io::read_file: failed to open file: %s\n", file_path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    char* content = (char*)malloc(sizeof(char) * length + 1);
    if (content == NULL) {
        fprintf(stderr, "io::read_file: failed to allocate memory for file: %s\n", file_path);
        exit(1);
    }

    fread(content, sizeof(char), length, file);
    content[length] = '\0';

    fclose(file);

    return content;
}
