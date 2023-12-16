#include <stdio.h>
#include <string.h>

#include "includes/memory.h"
#include "includes/object.h"
#include "includes/value.h"
#include "includes/vm.h"

#define ALLOCATE_OBJ(type, obj_type) ((type*)allocate_object(sizeof(type), obj_type))

static Obj* allocate_object(size_t size, ObjType type) {
    Obj* obj = (Obj*)reallocate(NULL, 0, size);
    obj->type = type;
    return obj;
}

static ObjString* allocate_string(char* heap_chars, int length) {
    ObjString* obj_str = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    obj_str->length = length;
    obj_str->chars = heap_chars;
    return obj_str;
}

ObjString* take_string(char* chars, int length) {
    return allocate_string(chars, length);
}

ObjString* copy_string(const char* chars, int length) {
    char* heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length);
}

void print_obj(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: {
            printf("%s", AS_CSTRING(value));
            break;
        }
    }
}

