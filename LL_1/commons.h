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
#define RIGHT_ARROW "→"
#define G_S_START "S"
#define NIL "ε"

#ifdef _WIN32
#define STRTOK(str, delim, saveptr) strtok_s(str, delim, saveptr)
#else
#define STRTOK(str, delim, saveptr) strtok_r(str, delim, saveptr)
#endif


#define ERROR_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE]; // 根据需要的字符集大小定义
    int index; // 唯一编号
    int isEndOfWord; // 标记是否为单词的结尾
} TrieNode;

typedef struct Token {
    int idx;
    int line;
    enum TokenType type;
}Token;


// 定义产生式结构体
typedef struct {
    char* lhs;  // 左侧符号
    char* rhs;
    char** rhs_symbols;  // 右侧符号序列(不定数目)
    int rhs_len;  // 右侧符号的数量
} Production;

// 定义集合结构体
typedef struct {
    char** symbols;     // 指向符号字符串数组的指针
    int size;           // 集合中符号的当前数量
    int capacity;       // 集合的容量
} SymbolSet;

typedef struct {
    Production* p;      // 产生式
    SymbolSet* symset;  // 对应的SELECT符号集合 
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


// 计算FIRST集
void compute_first_sets();
// 计算FOLLOW集
void compute_follow_sets();
// 计算SELECT集
void compute_select_sets();

// 解析产生式(左)
void parse_production_lhs(const char* production_str, Production* production);
// 解析产生式(右)
void parse_production_rhs(const char* production_str, Production* production);

// 去除字符串首尾空格, 会 malloc 一块新内存，用完需释放
char* trimSpaces(char* str);
// 将某个字符串放入字典树里, 不会重复插入, 返回该字符串的唯一的编号
int insertAndGetIndex(TrieNode* root, char* key);
// 新开一个字典树的节点
TrieNode* newTrieNode();
#endif // !COMMONS_H
