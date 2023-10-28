#include "common_headers.h"
#define N 20 // Number of states

enum State_double trans_mat_double[N][N];

enum InputSet_double {
    Plus, Minus, Equal, Exclamation, LessThan, GreaterThan, Ampersand, Pipe, Other
};

enum State_double {
    _0, _AND, _PLUS, _MINUS, _EQ_LT_GT, _EXCLAM, _OR, _DOUBLE_OP, _ERR
};

void init_double_sep() {
    memset(trans_mat_double, -1, sizeof(trans_mat_double));

    trans_mat_double[_0][Ampersand] = _AND;
    trans_mat_double[_0][Plus] = _PLUS;
    trans_mat_double[_0][Minus] = _MINUS;
    trans_mat_double[_0][Equal] = _EQ_LT_GT;
    trans_mat_double[_0][LessThan] = _EQ_LT_GT;
    trans_mat_double[_0][GreaterThan] = _EQ_LT_GT;
    trans_mat_double[_0][Pipe] = _OR;
    trans_mat_double[_0][Exclamation] = _EXCLAM;

    trans_mat_double[_EXCLAM][Equal] = _DOUBLE_OP;   // !=
    trans_mat_double[_EXCLAM][Exclamation] = _DOUBLE_OP; // !!
    trans_mat_double[_EQ_LT_GT][Equal] = _DOUBLE_OP; // ==, <=, >=
    trans_mat_double[_EQ_LT_GT][LessThan] = _DOUBLE_OP; // <<
    trans_mat_double[_EQ_LT_GT][GreaterThan] = _DOUBLE_OP; // >>
    trans_mat_double[_AND][Ampersand] = _DOUBLE_OP;  // &&
    trans_mat_double[_PLUS][Plus] = _DOUBLE_OP;      // ++
    trans_mat_double[_MINUS][Minus] = _DOUBLE_OP;    // --
    trans_mat_double[_OR][Pipe] = _DOUBLE_OP;        // ||
}

enum InputSet_double categorize_double(char c) {
    switch (c) {
    case '+': return Plus;
    case '-': return Minus;
    case '=': return Equal;
    case '!': return Exclamation;
    case '<': return LessThan;
    case '>': return GreaterThan;
    case '&': return Ampersand;
    case '|': return Pipe;
    default: return Other;
    }
}

int isDoubleSeparator(const char* s) {
    enum State_double currentState = _0;
    while (*s) {
        enum InputSet_double input = categorize_double(*s);
        if (trans_mat_double[currentState][input] == -1) {
            return 0; // _ERR transition
        }
        currentState = trans_mat_double[currentState][input];
        s++;
    }
    return (currentState == _DOUBLE_OP);
}

int iden_double_sep() {
    init_double_sep();

    const char* testCases[] = {
        "++", "--", "==", "!=", "<=", ">=", "&&", "||", "!!", "<<", ">>",
        "+", "-", "=", "!", "<", ">", "&", "|", "+-", "-+", "+++"
    };
    size_t numCases = sizeof(testCases) / sizeof(testCases[0]);

    for (size_t i = 0; i < numCases; i++) {
        if (isDoubleSeparator(testCases[i])) {
            printf("'%s' is a double character separator.\n", testCases[i]);
        }
        else {
            printf("'%s' is NOT a double character separator.\n", testCases[i]);
        }
    }

    return 0;
}

int maccin() {
    return iden_double_sep();
}
