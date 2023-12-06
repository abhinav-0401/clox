#pragma once

#include "chunk.h"

void disassemble_chunk(Chunk* chunk, const char* name);
int disassemble_instruction(Chunk* chunk, int offset);

int simple_instruction(const char* name, int offset);
int constant_instruction(const char* name, Chunk* chunk, int offset);

void chunk_info(Chunk* chunk, const char* name);
