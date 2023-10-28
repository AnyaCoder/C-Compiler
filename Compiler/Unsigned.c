#include "common_headers.h"
#include "lexAnalyzer.h"

#define N 8
#define M 7
char trans_mat_unsigned[N][N];

enum InputSet_unsigned {
    Sign, Digit, Dot, Ee, Other, Blank
};
enum State_unsigned {
    _0, _136, _2, _36, _45, _5, _6
};

void init_unsigned() {
    memset(trans_mat_unsigned, -1, sizeof(trans_mat_unsigned));
    trans_mat_unsigned[_0][Blank] = _0;
    trans_mat_unsigned[_0][Digit] = _136;
    trans_mat_unsigned[_136][Digit] = _136;
    trans_mat_unsigned[_136][Dot] = _2;
    trans_mat_unsigned[_136][Ee] = _45;
    trans_mat_unsigned[_2][Digit] = _36;
    trans_mat_unsigned[_36][Digit] = _36;
    trans_mat_unsigned[_36][Ee] = _45;
    trans_mat_unsigned[_45][Sign] = _5;
    trans_mat_unsigned[_45][Digit] = _6;
    trans_mat_unsigned[_5][Digit] = _6;
    trans_mat_unsigned[_6][Digit] = _6;
}

enum InputSet categorize_unsigned(char c) {
    if (c >= '0' && c <= '9') return Digit;
    if (c == '+' || c == '-') return Sign;
    if (c == '.') return Dot;
    if (c == 'e' || c == 'E') return Ee;
    if (c == ' ' || c == '\t' || c == '\n') return Blank;
    return Other;
}

int isUnsignedNumber(const char* s) {
    enum State currentState = _0;
    while (*s) {
        enum InputSet input = categorize_unsigned(*s);
        if (trans_mat_unsigned[currentState][input] == -1) {
            return 0; // Invalid transition
        }
        currentState = trans_mat_unsigned[currentState][input];
        s++;
    }
    return (currentState == _136 || currentState == _36 || currentState == _6);
}

int iden_unum() {
    init_unsigned();

    const char* testCases[] = {
        "123", "456789", "0",
        "0.1", "123.456", ".123",
        "1e10", "123E5", "0.123e4", ".123E2",
        "e10", "123.", "123e", "123.e4", "+123",
        "-0.1", "1.2.3", "12e4.5", "abc", "123e+","123e-05","123e+09"
    };
    size_t numCases = sizeof(testCases) / sizeof(testCases[0]);

    for (size_t i = 0; i < numCases; i++) {
        if (isUnsignedNumber(testCases[i])) {
            printf("'%s' is an unsigned number.\n", testCases[i]);
        }
        else {
            printf("'%s' is NOT an unsigned number.\n", testCases[i]);
        }
    }

    return 0;
}
