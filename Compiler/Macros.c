#include "common_headers.h"
#define M 8 // Number of states for macro recognition

char trans_mat_macro[M][M];

enum InputSet_macro {
    Hash, Whitespace, Alpha, BackSlash, Newline, Other
};

enum State_macro {
    Start, HashSeen, DefineSeen, A_MacroValid, N_MacroValid, ValValid, ifValValid_1, ifValValid_2
};
// 仅讨论形式上的宏定义是否合法: # [a-zA-z0-9_] () ()
// 仅讨论一行宏定义
void init_macro() {
    memset(trans_mat_macro, -1, sizeof(trans_mat_macro));

    trans_mat_macro[Start][Hash] = HashSeen;
    trans_mat_macro[Start][Whitespace] = Start;

    trans_mat_macro[HashSeen][Whitespace] = HashSeen; // Allow any number of whitespaces after '#'
    trans_mat_macro[HashSeen][Alpha] = DefineSeen; // Start of "define" seen

    trans_mat_macro[DefineSeen][Alpha] = DefineSeen; // Continue seeing "define"
    trans_mat_macro[DefineSeen][Whitespace] = A_MacroValid; // After fully seeing "#define", it's a valid macro if followed by whitespace
    trans_mat_macro[A_MacroValid][Alpha] = A_MacroValid;
    trans_mat_macro[A_MacroValid][Other] = N_MacroValid;
    trans_mat_macro[N_MacroValid][Alpha] = N_MacroValid;
    trans_mat_macro[N_MacroValid][Other] = N_MacroValid;
    trans_mat_macro[A_MacroValid][Whitespace] = ValValid;
    trans_mat_macro[N_MacroValid][Whitespace] = ValValid;
    trans_mat_macro[ValValid][Alpha] = ValValid;
    trans_mat_macro[ValValid][Whitespace] = ValValid;
    trans_mat_macro[ValValid][Other] = ValValid;
    trans_mat_macro[ValValid][BackSlash] = ifValValid_1;
    trans_mat_macro[ifValValid_1][Newline] = ifValValid_2;
    trans_mat_macro[ifValValid_2][Alpha] = ValValid;
    trans_mat_macro[ifValValid_2][Other] = ValValid;
    
}

enum InputSet_macro categorize_macro(char c) {
    if (c == '#') return Hash;
    if (c == '\\') return BackSlash;
    if (c == '\n') return Newline;
    if (isspace(c)) return Whitespace; // using isspace from ctype.h
    if (isalpha(c) || c == '_') return Alpha; // using isalpha from ctype.h
    return Other;
}

int isMacro(const char* s) {
    enum State_macro currentState = Start;
    while (*s) {
        enum InputSet_macro input = categorize_macro(*s);
        if (trans_mat_macro[currentState][input] == -1) {
            return 0; // Invalid transition
        }
        currentState = trans_mat_macro[currentState][input];
        s++;
    }
    return (currentState == DefineSeen || currentState == A_MacroValid || currentState == N_MacroValid || currentState == ValValid);
}

int iden_macro() {
    init_macro();

    const char* testCases[] = {
        "#define", "# define", "define", " #define MACRO", "#defineMACRO", "#define MACRO", 
        "#define SQUARE(x) ((x) * (x))", "#a b c d"
    };
    size_t numCases = sizeof(testCases) / sizeof(testCases[0]);

    for (size_t i = 0; i < numCases; i++) {
        int result = isMacro(testCases[i]);
        if (result == 1) {
            printf("'%s' is a valid macro.\n", testCases[i]);
        }
        else {
            printf("'%s' is NOT a valid macro.\n", testCases[i]);
        }
    }

    return 0;
}

int maiacxn() {
    return iden_macro();
}