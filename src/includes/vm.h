#pragma once

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR,
} InterpretResult;

void init_vm();
void free_vm();

InterpretResult interpret(const char* src);
void push(Value value);
Value pop();