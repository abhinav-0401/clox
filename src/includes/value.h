#pragma once

#include "common.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArr;

void init_value_arr(ValueArr* arr);
void free_value_arr(ValueArr* arr);
void write_value_arr(ValueArr* arr, Value value);

void print_value(Value value);