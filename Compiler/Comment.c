#include "common_headers.h"
#define N 6 // Number of states

char trans_mat_comment[N][N];

enum InputSet_comment {
    Slash, Star, Other
};
enum State_comment {
    Initial, SingleSlash, SingleLineComment, MultiLineComment, MultiLineCommentEnd_1, MultiLineCommentEnd_2
};

void init_comment() {
    memset(trans_mat_comment, -1, sizeof(trans_mat_comment));

    trans_mat_comment[Initial][Slash] = SingleSlash;
    trans_mat_comment[Initial][Star] = MultiLineCommentEnd_1;
    trans_mat_comment[MultiLineCommentEnd_1][Slash] = MultiLineCommentEnd_2;

    trans_mat_comment[SingleSlash][Slash] = SingleLineComment;
    trans_mat_comment[SingleSlash][Star] = MultiLineComment;
    trans_mat_comment[MultiLineComment][Other] = MultiLineComment;
    trans_mat_comment[MultiLineComment][Star] = MultiLineCommentEnd_1;
    trans_mat_comment[MultiLineCommentEnd_1][Slash] = MultiLineCommentEnd_2;
}

enum InputSet_comment categorize_comment(char c) {
    switch (c) {
    case '/': return Slash;
    case '*': return Star;
    default: return Other;
    }
}

int isComment(const char* s) {
    enum State_comment currentState = Initial;
    while (*s) {
        enum InputSet_comment input = categorize_comment(*s);
        if (trans_mat_comment[currentState][input] == -1) {
            return 0; // Invalid transition
        }
        currentState = trans_mat_comment[currentState][input];
        s++;
    }
    return (currentState == SingleLineComment || currentState == MultiLineComment || currentState == MultiLineCommentEnd_2);
}

int iden_comment() {
    init_comment();

    const char* testCases[] = {
        "/", "//", "/* comment */", "/* comment ", "comment */", "/comment", "*/"
    };
    size_t numCases = sizeof(testCases) / sizeof(testCases[0]);

    for (size_t i = 0; i < numCases; i++) {
        int result = isComment(testCases[i]);
        if (result == 1) {
            printf("'%s' is a valid comment.\n", testCases[i]);
        }
        else {
            printf("'%s' is NOT a valid comment.\n", testCases[i]);
        }
    }

    return 0;
}

int mSDASain() {
    return iden_comment();
}