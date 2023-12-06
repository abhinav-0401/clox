#include <stdio.h>

#include "includes/common.h"
#include "includes/compiler.h"
#include "includes/scanner.h"

void compile(const char* src) {
    init_scanner(src);
    for (;;) {
        Token token = scan_token();
        if (token.type == TOKEN_EOF) {
            break;
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);
    }

}