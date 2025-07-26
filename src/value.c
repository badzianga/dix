#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "value.h"

void print_value(Value value) {
    switch (value.type) {
        case VALUE_NONE:  printf("NONE"); break;
        case VALUE_BOOL:  printf("%s", AS_BOOL(value) ? "true" : "false"); break;
        case VALUE_INT:   printf("%d", AS_INT(value)); break;
        case VALUE_FLOAT: printf("%f", AS_FLOAT(value)); break;
        default: break;
    }
}

void push_to_value_array(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count++] = value;
}

void free_value_array(ValueArray* array) {
    free(array->values);

    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}
