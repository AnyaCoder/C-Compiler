#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ERROR_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#define ErrMsg(exp_str) ERROR_PRINT("Line %d: Syntax Error! Found: \"%s\",  Expected: \"%s\" . Skipped~\n", \
                token.line, str_TokenType[token.type], exp_str)

#define EOS_RET do {if (token.type == EOS) {currentToken--; return;}} while(0)

enum TokenType {
    IF, THEN, WHILE, DO, 
    BEGIN, END, VAR, L_BRANKET, R_BRANKET,
    SEMICOLON, ASSIGN, PLUS, MINUS, MUL, DIV, EOS,
    UNKNOWN
};

const char* str_TokenType[] = {
    "IF", "THEN", "WHILE", "DO",
    "BEGIN", "END", "VAR", "L_BRANKET", "R_BRANKET",
    "SEMICOLON", "ASSIGN", "PLUS", "MINUS", "MUL", "DIV", "EOS",
    "UNKNOWN"
};
typedef struct Token {
    int idx;
    int line;
    enum TokenType type;
}Token;

struct Token tokens[250]; // 当前处理的词法单元序列
int currentToken = 0;

void parse_S();
void parse_L();
void parse_LPrime();
void parse_APrime();
void parse_E();
void parse_EPrime();
void parse_T();
void parse_TPrime();
void parse_F();
void parse_A();
void parse_M();
void parse_V();

struct Token getNextToken() {
    
    return tokens[currentToken++];
}

void expect(enum TokenType type) {
    struct Token token = getNextToken();
    while (token.type != type) { // 不是期望符号，不断跳过
        EOS_RET; // 遇到EOS，直接返回
        ErrMsg(str_TokenType[type]); // 打印错误信息，期望符号
        token = getNextToken(); // 获取下一token
    }
    // 打印匹配了第几行的符号
    printf("(%d): %s\n", token.line, str_TokenType[token.type]);
}

void parse_S() {
    Token token = getNextToken();
    switch (token.type) // Token种类
    {
    case IF:
        currentToken--;
        expect(IF); // 判断当前token是否为if
        parse_E();  // 解析非终结符E
        expect(THEN); // 判断当前token是否为then
        parse_S(); // 解析非终结符S
        break;
    case WHILE:
        currentToken--;
        expect(WHILE);
        parse_E();
        expect(DO);
        parse_S();
        break;
    case BEGIN:
        currentToken--;
        expect(BEGIN);
        parse_L();
        expect(END);
        break;
    case VAR:
        currentToken--;
        parse_APrime();
        break;
    case END:
    case SEMICOLON:
    case EOS:
        currentToken--; // 若M[A,a]为synch, 弹出非终结符S，继续分析后面的
        ErrMsg("if|while|begin|VAR");
        break;
    default:
        ErrMsg("if|while|begin|VAR");    // 若M[A,a]为“空”, 报错, 忽略输入符号a，继续解析 
        EOS_RET;
        parse_S();
        break;
    }
}

void parse_L() {
    Token token = getNextToken();
    switch (token.type)
    {
    case IF:
    case WHILE:
    case BEGIN:
    case VAR:
        currentToken--;
        parse_S();
        parse_LPrime();
        break;
    case END:
        currentToken--;
        ErrMsg("if|while|begin|VAR");
        break;
    default:
        ErrMsg("if|while|begin|VAR");
        EOS_RET;
        parse_L();
        break;
    }
}

void parse_LPrime() {
    Token token = getNextToken();
    switch (token.type)
    {
    case END:
        currentToken--;
        break;
    case SEMICOLON:
        currentToken--;
        expect(SEMICOLON);
        parse_S();
        parse_LPrime();
        break;
    default:
        ErrMsg("end|;");
        EOS_RET;
        parse_LPrime();
        break;
    }
}

void parse_APrime() {
    Token token = getNextToken();
    switch (token.type)
    {
    case VAR:
        currentToken--;
        parse_V();
        expect(ASSIGN);
        parse_E();
        break;
    case END:
    case SEMICOLON:
    case EOS:
        currentToken--;
        ErrMsg("VAR");
        break;
    default:
        ErrMsg("VAR");
        EOS_RET;
        parse_APrime();
        break;
    }
}

void parse_E() {
    Token token = getNextToken();
    switch (token.type)
    {
    case VAR:
    case L_BRANKET:
        currentToken--;
        parse_T();
        parse_EPrime();
        break;
    case THEN:
    case DO:
    case END:
    case R_BRANKET:
    case SEMICOLON:
    case EOS:
        currentToken--;
        ErrMsg("VAR|(");
        break;
    default:
        ErrMsg("VAR|(");
        EOS_RET;
        parse_E();
        break;
    }
}

void parse_EPrime() {
    Token token = getNextToken();
    switch (token.type)
    {
    case THEN:
    case DO:
    case END:
    case R_BRANKET:
    case SEMICOLON:
    case EOS:
        currentToken--;
        break;
    case PLUS:
    case MINUS:
        currentToken--;
        parse_A();
        parse_T();
        parse_EPrime();
        break;
    default:
        ErrMsg("then|do|end|)|$|+|-");
        EOS_RET;
        parse_EPrime();
        break;
    }
}

void parse_T() {
    Token token = getNextToken();
    switch (token.type)
    {
    case VAR:
    case L_BRANKET:
        currentToken--;
        parse_F();
        parse_TPrime();
        break;
    case THEN:
    case DO:
    case END:
    case R_BRANKET:
    case SEMICOLON:
    case PLUS:
    case MINUS:
    case EOS:
        currentToken--;
        ErrMsg("VAR|(");
        break;
    default:
        ErrMsg("VAR|(");
        EOS_RET;
        parse_T();
        break;
    }
}

void parse_TPrime() {
    Token token = getNextToken();
    switch (token.type)
    {
    case THEN:
    case DO:
    case END:
    case R_BRANKET:
    case SEMICOLON:
    case EOS:
    case PLUS:
    case MINUS:
        currentToken--;
        break;
    case MUL:
    case DIV:
        currentToken--;
        parse_M();
        parse_F();
        parse_TPrime();
        break;
    default:
        ErrMsg("then|do|end|)|;|$|+|-|*|/");
        EOS_RET;
        parse_TPrime();
        break;
    }
}

void parse_F() {
    Token token = getNextToken();
    switch (token.type)
    {
    case VAR:
        currentToken--;
        expect(VAR);
        break;
    case L_BRANKET:
        currentToken--;
        expect(L_BRANKET);
        parse_E();
        expect(R_BRANKET);
        break;
    case THEN:
    case DO:
    case END:
    case R_BRANKET:
    case SEMICOLON:
    case PLUS:
    case MINUS:
    case MUL:
    case DIV:
    case EOS:
        currentToken--;
        ErrMsg("VAR|(");
        break;
    default:
        ErrMsg("VAR|(");
        EOS_RET;
        parse_F();
        break;
    }
}


void parse_A() {
    Token token = getNextToken();
    switch (token.type)
    {
    case PLUS:
        currentToken--;
        expect(PLUS);
        break;
    case MINUS:
        currentToken--;
        expect(MINUS);
        break;
    case VAR:
    case L_BRANKET:
        currentToken--;
        ErrMsg("+|-");
        break;
    default:
        ErrMsg("+|-");
        EOS_RET;
        parse_A();
        break;
    }
}

void parse_M() {
    Token token = getNextToken();
    switch (token.type)
    {
    case MUL:
        currentToken--;
        expect(MUL);
        break;
    case DIV:
        currentToken--;
        expect(DIV);
        break;
    case VAR:
    case L_BRANKET:
        currentToken--;
        ErrMsg("*|/");
        break;
    default:
        ErrMsg("*|/");
        EOS_RET;
        parse_M();
        break;
    }
}

void parse_V() {
    Token token = getNextToken();
    switch (token.type)
    {
    case VAR:
        currentToken--;
        expect(VAR);
        break;
    case ASSIGN:
        currentToken--;
        ErrMsg("VAR");
        break;
    default:
        ErrMsg("VAR");
        EOS_RET;
        parse_V();
        break;
    }
}

void idx_to_token(struct Token* pToken) {
    switch ((*pToken).idx)
    {
    case 15:
        (*pToken).type = IF;
        break;
    case 23:
        (*pToken).type = DO;
        break;
    case 24:
        (*pToken).type = WHILE;
        break;
    case 29:
        (*pToken).type = THEN;
        break;
    case 30:
        (*pToken).type = BEGIN;
        break;
    case 31:
        (*pToken).type = END;
        break;
    case 32:
        (*pToken).type = VAR;
        break;
    case 34:
        (*pToken).type = PLUS;
        break;
    case 35:
        (*pToken).type = MINUS;
        break;
    case 36:
        (*pToken).type = ASSIGN;
        break;
    case 42:
        (*pToken).type = MUL;
        break;
    case 43:
        (*pToken).type = DIV;
        break;
    case 52:
        (*pToken).type = L_BRANKET;
        break;
    case 53:
        (*pToken).type = R_BRANKET;
        break;
    case 54:
        (*pToken).type = SEMICOLON;
        break;
    default:
        (*pToken).type = UNKNOWN;
        break;
    }
}

void test() {
    currentToken = 0;
    parse_S();
    if (tokens[currentToken].type == EOS) {
        printf("Parsing Successful\n");
    }
    else {
        printf("The source file has Syntax Errors!\n");
    }
}
int main() {
    printf("Made by zrz\n");
    puts("请输入词法分析文本路径: ");
    unsigned char in_file_name[256];
    scanf("%255s", in_file_name);
    FILE* in_file = fopen(in_file_name, "r");
    if (!in_file) {
        perror("Error opening file");
        return 1;
    }
    Token token;
    unsigned char lexeme[34];
    int totalTokens = 0;
    while (fscanf(in_file, "%d, %d, %s\n", &token.line, &token.idx, lexeme) != EOF) {
        idx_to_token(&token);
        if (token.type != UNKNOWN) {
            tokens[totalTokens++] = token;
            printf("(%d): %s\n", token.line, lexeme);
        }
    }
    token.type = EOS;
    tokens[totalTokens] = token;
    fclose(in_file);
    test();

    return 0;
}