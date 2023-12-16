#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "includes/vm.h"
#include "includes/chunk.h"
#include "includes/value.h"
#include "includes/compiler.h"
#include "includes/debug.h"
#include "includes/object.h"
#include "includes/memory.h"

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

static bool is_falsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)) || (IS_NUMBER(value) && (AS_NUMBER(value) == 0));
}

static void concatenate() {
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = take_string(chars, length);
    push(OBJ_VAL(result));
}

static bool values_equal(Value a, Value b) {
    if (a.type != b.type) { return false; }
    switch (a.type) {
        case VAL_NIL: { return true; }
        case VAL_BOOL: { return AS_BOOL(a) == AS_BOOL(b);}
        case VAL_NUMBER: { return AS_NUMBER(a) == AS_NUMBER(b); }
        default: return false;
    }
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
                break;
            }
            case OP_NIL: { push(NIL_VAL); break; }
            case OP_TRUE: { push(BOOL_VAL(true)); break; }
            case OP_FALSE : { push(BOOL_VAL(false)); break; }
            case OP_NOT: {
                push(BOOL_VAL(is_falsey(pop())));
                break;
            }
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(0))) {
                    runtime_error("operand must be a number");
                    return INTERPRET_RUNTIME_ERR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            case OP_ADD: {
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                }
                else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                }
                else {
                    runtime_error("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERR;
                }
                break;
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
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(values_equal(a, b)));
                break;
            }
            case OP_GREATER: {
                BINARY_OP(BOOL_VAL, >);
                break;
            }
            case OP_LESS: {
                BINARY_OP(BOOL_VAL, <);
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