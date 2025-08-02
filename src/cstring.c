#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"

CString* create_cstring(const char* data) {
    int len = strlen(data);
    CString* cstring = malloc(sizeof(CString) + (len + 1) * sizeof(char));
    if (cstring == NULL) {
        fprintf(stderr, "error: cannot allocate memory for CString\n");
        exit(1);
    }
    memcpy(cstring->data, data, len);
    return cstring;
}

void free_cstring(CString* cstring) {
    free(cstring);
}

bool cstrings_equal(CString* str1, CString* str2) {
    return str1->length == str2->length && memcmp(str1->data, str2->data, str1->length) == 0;
}
