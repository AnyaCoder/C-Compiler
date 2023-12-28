#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_SYMBOLS 30
#define MAX_TERMINALS 10
#define MAX_PRODUCTIONS 10
#define MAX_PRODUCTION_LEN 10


typedef struct {
    char non_terminal;
    char productions[MAX_PRODUCTIONS][MAX_PRODUCTION_LEN];
    int production_count;
} GrammarRule;

typedef struct {
    char elements[MAX_SYMBOLS];
    char non_terminal;
    int size;
} Set;

typedef struct Token {
    int idx;
    int line;
    char type;
}Token;

enum TokenType {
    S, L, LPrime, APrime, E, EPrime, T, TPrime, F, A, M, V,
    IF = 12, THEN, WHILE, DO, BEGIN, END, SEMICOLON,
    ASSIGN, L_BRANKET, R_BRANKET, VAR, PLUS, MINUS, MUL, DIV, EOS, UNKNOWN
};

void idx_to_token(struct Token* pToken);

// Global Grammar Rules
Set firstvt_set[MAX_PRODUCTIONS], lastvt_set[MAX_PRODUCTIONS];
GrammarRule grammar_rules[MAX_SYMBOLS];
bool visited_first[MAX_SYMBOLS];
bool visited_last[MAX_SYMBOLS];
char terminals[MAX_SYMBOLS];
int grammar_rule_count = 0;
int terminal_count = 0;
Token tokens[250]; // 当前处理的词法单元序列
int currentToken = 0;   // 当前读入token数
int totalTokens = 0;    // 总token数


void init_set(Set* set, char non_term) {
    set->size = 0;
    memset(set->elements, 0, sizeof(set->elements));
    set->non_terminal = non_term;
}

void addToSet(Set* set, char element) {
    for (int i = 0; i < set->size; i++) {
        if (set->elements[i] == element) {
            return; // Element already in set
        }
    }
    set->elements[set->size++] = element; // Add new element
}

bool is_non_terminal(char symbol) {
    return symbol >= 'A' && symbol <= 'Z';
}

void firstvt(char non_terminal, Set* firstvt_set, bool* visited) {
    if (visited[non_terminal - 'A']) {
        return;
    }

    visited[non_terminal - 'A'] = true;

    for (int i = 0; i < grammar_rule_count; i++) {
        if (grammar_rules[i].non_terminal == non_terminal) {
            for (int j = 0; j < grammar_rules[i].production_count; j++) {
                char* production = grammar_rules[i].productions[j];
                char first_symbol = production[0];

                if (!is_non_terminal(first_symbol)) {
                    addToSet(firstvt_set, first_symbol);
                }
                else if (strlen(production) > 1 && !is_non_terminal(production[1])) {
                    addToSet(firstvt_set, production[1]);
                }
                else if (is_non_terminal(first_symbol)) {
                    Set temp_set;
                    init_set(&temp_set, non_terminal);
                    firstvt(first_symbol, &temp_set, visited);
                    for (int k = 0; k < temp_set.size; k++) {
                        addToSet(firstvt_set, temp_set.elements[k]);
                    }
                }
            }
        }
    }
}

void lastvt(char non_terminal, Set* lastvt_set, bool* visited) {
    if (visited[non_terminal - 'A']) {
        return;
    }

    visited[non_terminal - 'A'] = true;

    for (int i = 0; i < grammar_rule_count; i++) {
        if (grammar_rules[i].non_terminal == non_terminal) {
            for (int j = 0; j < grammar_rules[i].production_count; j++) {
                char* production = grammar_rules[i].productions[j];
                int len = strlen(production);
                char last_symbol = production[len - 1];

                if (!is_non_terminal(last_symbol)) {
                    addToSet(lastvt_set, last_symbol);
                }
                else if (len > 1 && !is_non_terminal(production[len - 2])) {
                    addToSet(lastvt_set, production[len - 2]);
                }
                else if (is_non_terminal(last_symbol)) {
                    Set temp_set;
                    init_set(&temp_set, non_terminal);
                    lastvt(last_symbol, &temp_set, visited);
                    for (int k = 0; k < temp_set.size; k++) {
                        addToSet(lastvt_set, temp_set.elements[k]);
                    }
                }
            }
        }
    }
}

void print_set(Set* set) {
    for (int i = 0; i < set->size; i++) {
        printf("%c ", set->elements[i]);
    }
    printf("\n");
}


void add_rule(char non_terminal, const char* production_str) {
    GrammarRule* rule = &grammar_rules[grammar_rule_count++];
    rule->non_terminal = non_terminal;
    rule->production_count = 0;

    const char* token;
    const char* delim = "|";
    char* production_str_copy = _strdup(production_str);
    token = strtok(production_str_copy, delim);

    while (token != NULL) {
        strcpy(rule->productions[rule->production_count++], token);
        token = strtok(NULL, delim);
    }

    free(production_str_copy);
}

// 提取终结符函数
void extract_terminals(char terminals[]) {
    int index = 0;
    for (int i = 0; i < grammar_rule_count; i++) {
        for (int j = 0; j < MAX_PRODUCTIONS && grammar_rules[i].productions[j][0] != '\0'; j++) {
            for (int k = 0; k < MAX_PRODUCTION_LEN && grammar_rules[i].productions[j][k] != '\0'; k++) {
                char symbol = grammar_rules[i].productions[j][k];
                if (!isupper(symbol) && strchr(terminals, symbol) == NULL) {
                    terminals[index++] = symbol;
                    if (index >= MAX_TERMINALS - 1) {
                        printf("Error: Too many terminals.\n");
                        exit(1);
                    }
                }
            }
        }
    }
    terminals[index] = '\0';  // 字符串结束符
    terminal_count = index;
}

// 算符优先矩阵
char op_matrix[MAX_TERMINALS][MAX_TERMINALS];


int find_terminal_index(char* terminals, char terminal) {
    for (int i = 0; terminals[i] != '\0'; i++) {
        if (terminals[i] == terminal) return i;
    }
    return -1;
}

int find_non_terminal_index(Set* vt, char nt) {
    for (int i = 0; i < grammar_rule_count; i++) {
        if (vt[i].non_terminal == nt) return i;
    }
    return -1;
}

void build_operator_precedence_matrix(Set* firstvt, Set* lastvt, char terminals[]) {

    int num_terminals = strlen(terminals);
    // 置空
    for (int i = 0; i < num_terminals; i++) {
        for (int j = 0; j < num_terminals; j++) {
            op_matrix[i][j] = ' ';
        }
    }

    for (int i = 0; i < grammar_rule_count; i++) {
        char non_terminal = grammar_rules[i].non_terminal;

        for (int j = 0; j < grammar_rules[i].production_count; j++) {
            char* production = grammar_rules[i].productions[j];
            int len = strlen(production);

            for (int k = 0; k < len - 1; k++) {
                char curr = production[k];
                char next = production[k + 1];
                
                // 终结符紧跟着非终结符, U -> aV...
                if (!is_non_terminal(curr) && is_non_terminal(next)) {
                    int curr_index = find_terminal_index(terminals, curr);
                    int nt_index = find_non_terminal_index(firstvt, next);
                    for (int l = 0; l < firstvt[nt_index].size; l++) {
                        int next_index = find_terminal_index(terminals, firstvt[nt_index].elements[l]);
                        if (curr_index != -1 && next_index != -1) {
                            op_matrix[curr_index][next_index] = '<';
                        }
                    }
                }

                // 非终结符紧跟着终结符
                if (is_non_terminal(curr) && !is_non_terminal(next)) {
                    int nt_index = find_non_terminal_index(lastvt, curr);
                    for (int l = 0; l < lastvt[nt_index].size; l++) {
                        int curr_index = find_terminal_index(terminals, lastvt[nt_index].elements[l]);
                        int next_index = find_terminal_index(terminals, next);
                        if (curr_index != -1 && next_index != -1) {
                            op_matrix[curr_index][next_index] = '>';
                        }
                    }
                }
            }
        }
    }

    // 对于相邻的终结符， 设置为等号
    for (int i = 0; i < grammar_rule_count; i++) {
        for (int j = 0; j < grammar_rules[i].production_count; j++) {
            char* production = grammar_rules[i].productions[j];
            int len = strlen(production);
            for (int k = 0; k < len - 1; k++) {
                // U -> ...ab...
                if (!is_non_terminal(production[k]) && !is_non_terminal(production[k + 1])) {
                    int index1 = find_terminal_index(terminals, production[k]);
                    int index2 = find_terminal_index(terminals, production[k + 1]);
                    if (index1 != -1 && index2 != -1) {
                        op_matrix[index1][index2] = '=';
                    }
                }
                if (k > 0) {
                    // U -> ...aVb...
                    if (!is_non_terminal(production[k - 1])
                        && !is_non_terminal(production[k + 1])
                        && is_non_terminal(production[k])) {
                        int index1 = find_terminal_index(terminals, production[k - 1]);
                        int index2 = find_terminal_index(terminals, production[k + 1]);
                        op_matrix[index1][index2] = '=';
                    }
                }
            }
        }
    }

    for (int i = 0; i < terminal_count; i++) {
        for (int j = 0; j < terminal_count; j++) {
            if (op_matrix[i][j] == ' ') {
                op_matrix[i][j] = 'N';
            }
        }
    }
}

#define MAX_STACK_SIZE 100
#define MAX_TERMINALS 100


// 栈操作
char stack[MAX_STACK_SIZE];
int stack_top = -1;

void push(char c) {
    if (stack_top < MAX_STACK_SIZE - 1) {
        stack[++stack_top] = c;
    }
    else {
        printf("Stack Overflow\n");
    }
}

void pop() {
    if (stack_top >= 0) {
        stack[stack_top--] = '\0';
    }
    else {
        printf("Stack Underflow\n");
    }
}

char top() {
    if (stack_top >= 0) {
        return stack[stack_top];
    }
    else {
        return '\0'; // 空栈
    }
}


char get_top_terminal() {
    for (int i = stack_top; i >= 0; i--) {
        if (!is_non_terminal(stack[i])) {
            return stack[i];
        }
    }
    return '\0';
}

bool reduce_stack(char cur) {
    int reduced = false;

    for (int k = 3; k >= 1; k--) {
        for (int i = 0; i < grammar_rule_count; i++) {
            for (int j = 0; j < grammar_rules[i].production_count; j++) {
           
                char* production = grammar_rules[i].productions[j];
                int len = strlen(production);
                if (len != k) continue;
                char* stack_suffix = stack + stack_top + 1 - len;
                int diff = 0;
                for (int t = 0; stack_suffix[t] != '\0'; t++) {
                    if (is_non_terminal(stack_suffix[t]) 
                        && is_non_terminal(production[t])) {
                        continue;
                    }
                    diff += (stack_suffix[t] != production[t]);
                }
                if (stack_top >= len && !diff) {
                    char tmp[20];
                    sprintf(tmp, "归约: %c -> %s", grammar_rules[i].non_terminal, stack_suffix);
                    printf("%-20s\n", tmp);
                    for (int j = 0; j < len; j++) {
                        pop();
                    }
                    
                    push(grammar_rules[i].non_terminal);
                    reduced = true;
                    goto outloop;
                }
            }
            
        }
    }
outloop:;
    return reduced;
}

bool operator_precedence_parse(char* input_string, char start_symbol) {
    push('#');
    

    int i = 0;
    int step = 0;
    printf("%-10s %-20s %-20s %-10s %-20s\n", "步骤", "符号栈", "输入串", "优先级", "动作");
    while (input_string[i] != '\0') {
        char top_terminal = get_top_terminal();
        char cur = input_string[i];
        int top_index = find_terminal_index(terminals, top_terminal);
        int cur_index = find_terminal_index(terminals, cur);
        char priority = op_matrix[top_index][cur_index];
      
        printf("%-10d ", step);
        printf("%-20s ", stack);
        printf("%-20s ", input_string + i);
        char tmp[] = {top_terminal, priority, cur, '\0'};
        printf("%-10s ", tmp);
        step++;
        if (top_terminal == '\0' || priority  == ' ') {
            return false; // 无法比较优先级，拒绝输入串
        }
        
        if (priority == '<' || priority == '=') {
            push(cur);
            char temp[20];
            sprintf(temp, "移入: %c", cur);
            printf("%-20s\n", temp);
            i++;
        }
        else if (priority == '>') {
            bool reduced = reduce_stack(cur);
            if (reduced == false) {
                printf("无法完成任何归约，出错!\n");
                return false;
            }
        }
        else {
            printf("优先矩阵对应为空！出错!\n");
            return false;
        }

    }
  
    return (stack_top == 2 && stack[0] == start_symbol && stack[2] == start_symbol);
}


int main() {
    // Initialize grammar rules from the given strings
    // 拓广文法

    add_rule('E', "E+T|E-T|T");
    add_rule('T', "T*F|T/F|F");
    add_rule('F', "(E)|i");
    add_rule('#', "#E#");
    extract_terminals(terminals);

    for (int i = 0; i < grammar_rule_count; i++) {

        char cur_NT = grammar_rules[i].non_terminal;
        init_set(&firstvt_set[i], cur_NT);
        init_set(&lastvt_set[i], cur_NT);
        firstvt_set[i].non_terminal = cur_NT;
        lastvt_set[i].non_terminal = cur_NT;
        memset(visited_first, 0, sizeof(visited_first));
        memset(visited_last, 0, sizeof(visited_last));
        firstvt(cur_NT, &firstvt_set[i], visited_first);
        lastvt(cur_NT, &lastvt_set[i], visited_last);
    }
    puts("\n \033[33m 当前文法的FIRSTVT集为：\033[0m");
    for (int i = 0; i < grammar_rule_count; i++) {
        char cur_NT = grammar_rules[i].non_terminal;
        printf("%c: ", cur_NT);
        print_set(&firstvt_set[i]);

    }
    puts("\n \033[33m 当前文法的LASTVT集为：\033[0m");
    for (int i = 0; i < grammar_rule_count; i++) {
        char cur_NT = grammar_rules[i].non_terminal;
        printf("%c: ", cur_NT);
        print_set(&lastvt_set[i]);
    }

    build_operator_precedence_matrix(firstvt_set, lastvt_set, terminals);
    puts("\n \033[33m 当前算符优先文法的优先关系矩阵为：\033[0m");
    int t_len = strlen(terminals);
    for (int i = 0; i < t_len; i++) {
        if (i == 0) {
            printf("  ");
            for (int j = 0; j < t_len; j++) {
                printf(" %c", terminals[j]);
            }
            printf("\n");
        }
        printf("%c ", terminals[i]);

        for (int j = 0; j < t_len; j++) {
            printf(" %c", op_matrix[i][j]);
        }
        printf("\n");
    }
    /***************** 初始化完成 *************/
    printf("实验四：算符优先文法Made by zrz\n");
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

    while (fscanf(in_file, "%d, %d, %s\n", &token.line, &token.idx, lexeme) != EOF) {
        idx_to_token(&token);
        if (token.type != UNKNOWN) {
            tokens[totalTokens++] = token;
            printf("(%d): %s\n", token.line, lexeme);
        }
    }
    token.type = '#';
    tokens[totalTokens] = token;

    fclose(in_file);
    char input_string[200] = { 0 };
    for (int i = 0; i < totalTokens; i++) {
        input_string[i] = tokens[i].type;
    }
    char start_symbol = '#';
    strcat(input_string, "#");
    bool is_valid = operator_precedence_parse(input_string, start_symbol);
    if (is_valid) { 
        printf("输入串 \033[32m 符合 \033[0m 这个算符优先文法规则！\n");
    }
    else {
        printf("输入串 \033[31m 不符合 \033[0m 这个算符优先文法规则！\n");
    }
    return 0;
}

void idx_to_token(struct Token* pToken) {
    switch ((*pToken).idx)
    {

    case 32:
        (*pToken).type = 'i';
        break;
    case 34:
        (*pToken).type = '+';
        break;
    case 35:
        (*pToken).type = '-';
        break;
    case 42:
        (*pToken).type = '*';
        break;
    case 43:
        (*pToken).type = '/';
        break;
    case 52:
        (*pToken).type = '(';
        break;
    case 53:
        (*pToken).type = ')';
        break;
    default:
        (*pToken).type = UNKNOWN;
        break;
    }
}