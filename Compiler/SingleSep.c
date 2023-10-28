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
    case '~':
    case '"':
    case '\'':
    case '\\':
    case '.':
        return 1;
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
