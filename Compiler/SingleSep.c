#include "common_headers.h"

// 跳表会更快
int isSingleSeparator(char c) {
    switch (c) {
    case '+':
    case '-':
    case '=':
    case '!':
    case '<':
    case '>':
    case '&':
    case '|':
    case '*':
    case '/':
    case '%':
    case '^':
    case '\'':
    case '\\':
    case '[':
    case ']':
    case '{':
    case '}':
    case '(':
    case ')':
    case ';':
    case ':':
    case ',':
    case '?':
    case '"':
    case '.':
    case '~':
        return 1;
    default:
        return 0;
    }
}

int get_idx_single(char c) {
    switch (c) {
    case '+': return 34;
    case '-': return 35;
    case '=': return 36;
    case '!': return 37;
    case '<': return 38;
    case '>': return 39;
    case '&': return 40;
    case '|': return 41;
    case '*': return 42;
    case '/': return 43;
    case '%': return 44;
    case '^': return 45;
    case '\'': return 46;
    case '\\': return 47;
    case '[': return 48;
    case ']': return 49;
    case '{': return 50;
    case '}': return 51;
    case '(': return 52;
    case ')': return 53;
    case ';': return 54;
    case ':': return 55;
    case ',': return 56;
    case '?': return 57;
    case '"': return 58;
    case '.': return 59;
    case '~': return 60;
    case '#': return 61;
    default:
        return 0;
    }
}
int iden_single_sep() {
    const char testCases[] = {
        '+', '-', '=', '!', '<', '>', '&', '|', '*', '/', '%',
        '^', '[', ']', '{', '}', '(', ')', ';', ':', ',', '?', '~',
        'a', '1', ' '
    };
    size_t numCases = sizeof(testCases) / sizeof(testCases[0]);

    for (size_t i = 0; i < numCases; i++) {
        if (isSingleSeparator(testCases[i])) {
            printf("'%c' is a single character separator.\n", testCases[i]);
        }
        else {
            printf("'%c' is NOT a single character separator.\n", testCases[i]);
        }
    }

    return 0;
}

int maixxn() {
    return iden_single_sep();
}
