#include <stdio.h>
#include "value.h"

void print_value(Value value) {
    switch (value.type) {
        case VALUE_NONE:  printf("NONE"); break;
        case VALUE_BOOL:  printf("%s\n", AS_BOOL(value) ? "true" : "false"); break;
        case VALUE_INT:   printf("%d\n", AS_INT(value)); break;
        case VALUE_FLOAT: printf("%f\n", AS_FLOAT(value)); break;
        default: break;
    }
}
