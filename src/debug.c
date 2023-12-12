#include <stdio.h>

#include "includes/debug.h"
#include "includes/chunk.h"
#include "includes/value.h"

void disassemble_chunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    // chunk_info(chunk, name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}

int disassemble_instruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    // printf("instruction: %d", instruction);
    switch (instruction) {
        case OP_RETURN:
            // printf("inside the OP_RETURN case");
            return simple_instruction("OP_RETURN", offset);
        case OP_CONSTANT:   // actually takes an operand, the index to the constant stored in the constant pool
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_NIL:
            return simple_instruction("OP_NIL", offset);
        case OP_TRUE:
            return simple_instruction("OP_TRUE", offset);
        case OP_FALSE:
            return simple_instruction("OP_FALSE", offset);
        case OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        case OP_NOT:
            return simple_instruction("OP_NOT", offset);
        case OP_ADD:    // even though the arithmetic operators take operands, the bytecode instructions DO NOT
            return simple_instruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simple_instruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simple_instruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simple_instruction("OP_DIVIDE", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

int simple_instruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int constant_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t const_idx = chunk->code[offset + 1];
    printf("%-16s %4d ", name, const_idx);
    Value constant = chunk->constants.values[const_idx];
    print_value(constant);
    printf("\n");
    return offset + 2;
}

void chunk_info(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    printf("chunk->count: %d\t\tchunk->capacity: %d\n", chunk->count, chunk->capacity);
    printf("ValueArr->count: %d\tValueArr->capacity: %d\n", chunk->constants.count, chunk->constants.capacity);
}