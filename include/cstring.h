#pragma once
#include <stdbool.h>

typedef struct CString {
    int length;
    char data[];
} CString;

CString* create_cstring(const char* data);
void free_cstring(CString* cstring);
bool cstrings_equal(CString* str1, CString* str2);
