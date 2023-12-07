#include <stdio.h>
#include <stdarg.h>

#include "includes/vm.h"
#include "includes/chunk.h"
#include "includes/value.h"
#include "includes/compiler.h"
#include "includes/debug.h"

VM vm;

static void reset_stack() {
    vm.stack_top = vm.stack;
}

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    //   int line = vm.chunk->lines[instruction];
    reset_stack();
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

static Value peek(int distance) {
    return vm.stack_top[-1-distance];
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[*vm.ip++])
#define BINARY_OP(value_type, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtime_error("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(value_type(a op b)); \
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
            case OP_NIL: { push(NIL_VAL); break; }
            case OP_TRUE: { push(BOOL_VAL(true)); break; }
            case OP_FALSE : { push(BOOL_VAL(false)); break; }
            case OP_NOT: {
                if (!IS_BOOL(pop())) {
                    runtime_error("operand must be a number");
                    return INTERPRET_RUNTIME_ERR;
                }
                push(BOOL_VAL(!AS_BOOL(pop())));
            }
            case OP_NEGATE: {
                if (!IS_NUMBER(pop())) {
                    runtime_error("operand must be a number");
                    return INTERPRET_RUNTIME_ERR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            case OP_ADD: {
                BINARY_OP(NUMBER_VAL, +); break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(NUMBER_VAL, -);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(NUMBER_VAL, *);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(NUMBER_VAL, /);
                break;
            }
        }        
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char* src) {
    Chunk chunk;
    init_chunk(&chunk);

    if(!compile(src, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERR;
    }    

    disassemble_chunk(&chunk, "Expression Chunk");

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;
    init_vm();

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
}