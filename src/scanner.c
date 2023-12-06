#include <stdio.h>
#include <string.h>

#include "includes/common.h"
#include "includes/scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

static bool is_at_end();
static Token make_token(TokenType type);
static Token error_token(const char* msg);
static char advance();
static char peek();
static char peek_next();
static Token string_token();
static Token number_token();
static Token ident_token();
static TokenType ident_type();
static bool is_digit(char c);
static bool is_alpha(char c);
static bool match(char expected);
static void skip_whitespace();

void init_scanner(const char* src) {
    scanner.start = src;
    scanner.current = src;
    scanner.line = 1;
}

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current;

    if (is_at_end()) {
        return make_token(TOKEN_EOF);
    }

    char c = advance();

    if (is_alpha(c)) {
        return ident_token();
    }

    if (is_digit(c)) {
        return number_token();
    }

    switch (c) {
        case '(':
            return make_token(TOKEN_LEFT_PAREN);
        case ')':
            return make_token(TOKEN_RIGHT_PAREN);
        case '{':
            return make_token(TOKEN_LEFT_BRACE);
        case '}':
            return make_token(TOKEN_RIGHT_BRACE);
        case ';':
            return make_token(TOKEN_SEMICOLON);
        case ',':
            return make_token(TOKEN_COMMA);
        case '.':
            return make_token(TOKEN_DOT);
        case '-':
            return make_token(TOKEN_MINUS);
        case '+':
            return make_token(TOKEN_PLUS);
        case '*':
            return make_token(TOKEN_STAR);
        case '!':
            return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '/':
            if (peek_next() == '/') {
                while (peek() != '/n' && !is_at_end()) {
                    advance();
                }
            } else {
                return make_token(TOKEN_SLASH);
            }
            break;
        case '"':
            return string_token();
        default:
            return;
    }

    return error_token("unexpected char");
}

static char advance() {
    char c = *scanner.current;
    ++scanner.current;
    return c;
}

static char peek() {
    return *scanner.current;
}

static char peek_next() {
    if (is_at_end()) {
        return '\0';
    }
    return scanner.current[1];
}

static Token string_token() {
    while (peek() != '"' && !is_at_end()) {
        advance();
    }

    if (is_at_end()) {
        return error_token("unterminated string");
    }

    advance();  // the final "
    return make_token(TOKEN_STRING);
}

static Token number_token() {
    while (is_digit(peek())) {
        advance();
    }

    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) {
            advance();
        }
    }
    return make_token(TOKEN_NUMBER);
}

static Token ident_token() {
    while (is_alpha(peek()) || is_digit(peek())) { advance(); } // after the first char, we allow digits in idents too

    return make_token(ident_type());
}

static TokenType ident_type() {
    return TOKEN_IDENTIFIER;
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_alpha(char c) {
    bool is_lower_case = (c >= 'a' && c <= 'z');
    bool is_upper_case = (c >= 'A' && c <= 'Z');
    return (is_lower_case || is_upper_case || c == '_');
}

static bool match(char expected) {
    if (is_at_end()) {
        return false;
    }
    if (*scanner.current != expected) {
        return false;
    }
    ++scanner.current;
    return true;
}

static bool is_at_end() {
    return *scanner.current == '\0';
}

static Token make_token(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    return token;
}

static Token error_token(const char* msg) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (int)strlen(msg);
    return token;
}

static void skip_whitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                advance();
                break;
            default:
                return;
        }
    }
}