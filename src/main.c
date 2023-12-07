#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/common.h"
#include "includes/chunk.h"
#include "includes/debug.h"
#include "includes/value.h"
#include "includes/vm.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void print_args(int argc, char** argv);
static void test_chunk();
static void repl();
static void run_file(const char* path);
static char* read_file(const char* path);

int main(int argc, char** argv) {
    // print_args(argc, argv);
    // test_chunk();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    return EXIT_SUCCESS;
}

static void repl() {
    char line[1024];
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static void run_file(const char* path) {
    char* src = read_file(path);
    InterpretResult result = interpret(src);
    free(src);

    switch (result) {
        case INTERPRET_COMPILE_ERR: exit(65);
        case INTERPRET_RUNTIME_ERR: exit(70);
    }
}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    buffer[bytes_read] = '\0';
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    fclose(file);
    return buffer;
}

static void test_chunk() {
    init_vm();

    Chunk chunk;
    init_chunk(&chunk);

    int new_const = add_constant(&chunk, NUMBER_VAL(5));
    write_chunk(&chunk, OP_CONSTANT);
    write_chunk(&chunk, new_const);

    write_chunk(&chunk, OP_NEGATE);

    int constant = add_constant(&chunk, NUMBER_VAL(3));
    write_chunk(&chunk, OP_CONSTANT);
    write_chunk(&chunk, constant);

    write_chunk(&chunk, OP_ADD);

    constant = add_constant(&chunk, NUMBER_VAL(10));
    write_chunk(&chunk, OP_CONSTANT);
    write_chunk(&chunk, constant);

    write_chunk(&chunk, OP_DIVIDE);

    write_chunk(&chunk, OP_RETURN);
    disassemble_chunk(&chunk, "First Chunk");
    // interpret(&chunk);

    free_vm();
    free_chunk(&chunk);
}

static void print_args(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        char *arg = argv[i];
        printf("%s ", argv[i]);
    }
    printf("\n");
}