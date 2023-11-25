#include "common_headers.h"
#define N 5
#define M 6
char trans_mat_iden[N][M];

enum InputSet_iden {
    Letter, Digit, Underscore, Other, Blank, End
};

enum State_iden {
    _0, _1, _2
};

void init_iden() {
    memset(trans_mat_iden, -1, sizeof(trans_mat_iden));
    trans_mat_iden[_0][Blank] = _0;
    trans_mat_iden[_0][Letter] = _1;
    trans_mat_iden[_0][Other] = _2;
    trans_mat_iden[_0][Underscore] = _1;
    trans_mat_iden[_1][Letter] = _1;
    trans_mat_iden[_1][Digit] = _1;
    trans_mat_iden[_1][Underscore] = _1;
    trans_mat_iden[_1][Blank] = _2;
    trans_mat_iden[_1][Other] = _2;
    

}

enum InputSet_iden categorize_iden(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return Letter;
    if (c >= '0' && c <= '9') return Digit;
    if (c == '_') return Underscore;
    if (c == ' ' || c == '\t' || c == '\n' || c == '\0') return Blank;
    return Other;
}

int isIdentifier(const char* s) {
    enum State_iden currentState = _0;
    while (*s) {
        enum InputSet_iden input = categorize_iden(*s);
        if (trans_mat_iden[currentState][input] == -1) {
            return 0; // Invalid transition
        }
        currentState = trans_mat_iden[currentState][input];
        s++;
    }
    return (currentState == _1);
}
