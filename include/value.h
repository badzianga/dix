#pragma once
#include <stdbool.h>

typedef enum ValueType {
    VALUE_NONE,
    VALUE_BOOL,
    VALUE_INT,
    VALUE_FLOAT,
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        bool bool_;
        int int_;
        float float_;
    } as;
} Value;

#define NONE_VALUE()       ((Value){ 0 })
#define BOOL_VALUE(value)  ((Value){ .type = VALUE_BOOL, { .bool_ = value } })
#define INT_VALUE(value)   ((Value){ .type = VALUE_INT, { .int_ = value } })
#define FLOAT_VALUE(value) ((Value){ .type = VALUE_FLOAT, { .float_ = value } })

#define AS_BOOL(value)     ((value).as.bool_)
#define AS_INT(value)      ((value).as.int_)
#define AS_FLOAT(value)    ((value).as.float_)

#define IS_BOOL(value)     ((value).type == VALUE_BOOL)
#define IS_INT(value)      ((value).type == VALUE_INT)
#define IS_FLOAT(value)    ((value).type == VALUE_FLOAT)

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

void print_value(Value value);

void push_to_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);
