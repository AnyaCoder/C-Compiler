#ifndef COMMONS_H
#define COMMONS_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MALLOC_NULL_EXIT(pStr) do {\
        if (pStr == NULL) { \
            fprintf(stderr, "(%d) in '%s': Cannot malloc!\n", __LINE__, __FUNCTION__); \
            exit(-1); \
        } \
    } while(0)

#define ALPHABET_SIZE 64
#define MAX_SYMBOLS 5
#define RIGHT_ARROW "��"
#define G_S_START "S"
#define NIL "��"

#ifdef _WIN32
#define STRTOK(str, delim, saveptr) strtok_s(str, delim, saveptr)
#else
#define STRTOK(str, delim, saveptr) strtok_r(str, delim, saveptr)
#endif


#define ERROR_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE]; // ������Ҫ���ַ�����С����
    int index; // Ψһ���
    int isEndOfWord; // ����Ƿ�Ϊ���ʵĽ�β
} TrieNode;

typedef struct Token {
    int idx;
    int line;
    enum TokenType type;
}Token;


// �������ʽ�ṹ��
typedef struct {
    char* lhs;  // ������
    char* rhs;
    char** rhs_symbols;  // �Ҳ��������(������Ŀ)
    int rhs_len;  // �Ҳ���ŵ�����
} Production;

// ���弯�Ͻṹ��
typedef struct {
    char** symbols;     // ָ������ַ��������ָ��
    int size;           // �����з��ŵĵ�ǰ����
    int capacity;       // ���ϵ�����
} SymbolSet;

typedef struct {
    Production* p;      // ����ʽ
    SymbolSet* symset;  // ��Ӧ��SELECT���ż��� 
} SelectSet;


typedef struct {
    SelectSet* select;
    int sync;
} TableEntry;

enum TokenType {
    S, L, LPrime, APrime, E, EPrime, T, TPrime, F, A, M, V,
    IF = 12, THEN, WHILE, DO, BEGIN, END, SEMICOLON,
    ASSIGN, L_BRANKET, R_BRANKET, VAR, PLUS, MINUS, MUL, DIV, EOS, UNKNOWN
};


// ����FIRST��
void compute_first_sets();
// ����FOLLOW��
void compute_follow_sets();
// ����SELECT��
void compute_select_sets();

// ��������ʽ(��)
void parse_production_lhs(const char* production_str, Production* production);
// ��������ʽ(��)
void parse_production_rhs(const char* production_str, Production* production);

// ȥ���ַ�����β�ո�, �� malloc һ�����ڴ棬�������ͷ�
char* trimSpaces(char* str);
// ��ĳ���ַ��������ֵ�����, �����ظ�����, ���ظ��ַ�����Ψһ�ı��
int insertAndGetIndex(TrieNode* root, char* key);
// �¿�һ���ֵ����Ľڵ�
TrieNode* newTrieNode();
#endif // !COMMONS_H
