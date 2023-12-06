#include <stddef.h>
#include <stdio.h>

#include "includes/value.h"
#include "includes/memory.h"

void init_value_arr(ValueArr* arr) {
    arr->count = 0;
    arr->capacity = 0;
    arr->values = NULL;
}

void free_value_arr(ValueArr* arr) {
    FREE_ARRAY(Value, arr, arr->capacity);
    init_value_arr(arr);
}

void write_value_arr(ValueArr* arr, Value value) {
    if (arr->capacity < arr->count + 1) {
        int old_cap = arr->capacity;
        arr->capacity = GROW_CAPACITY(old_cap);
        arr->values = GROW_ARRAY(Value, arr->values, old_cap, arr->capacity);
    }

    arr->values[arr->count] = value;
    ++arr->count;
}

void print_value(Value value) {
    printf("%g", value);
}