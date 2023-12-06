#include <stdio.h>

#include "includes/vm.h"
#include "includes/chunk.h"
#include "includes/value.h"
#include "includes/compiler.h"

VM vm;

static void reset_stack() {
    vm.stack_top = vm.stack;
}

void init_vm() {
    reset_stack();
}

void free_vm() {

}

void push(Value value) {
    *vm.stack_top = value;
    ++vm.stack_top;
}

Value pop() {
    --vm.stack_top;
    return *vm.stack_top;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[*vm.ip++])
#define BINARY_OP(op) \
    do { \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while (false)

    for(;;) {
        uint8_t instruction = READ_BYTE();
        switch (instruction) {
            case OP_RETURN: {
                print_value(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                // print_value(constant);
                // printf("\n");
                break;
            }
            case OP_NEGATE: {
                push(-pop());
                break;
            }
            case OP_ADD: {
                BINARY_OP(+); break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(-);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(*);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(/);
                break;
            }
        }        
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char* src) {
    compile(src);
    return INTERPRET_OK;
}