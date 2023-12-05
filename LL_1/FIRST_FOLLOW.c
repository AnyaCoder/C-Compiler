#include "commons.h"

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

// 定义全局变量
Production productions[100];  // 产生式数组
int production_count = 0;     // 产生式数量
SymbolSet first_sets[100];    // FIRST集合数组
SymbolSet follow_sets[100];   // FOLLOW集合数组
SelectSet select_sets[100];   // SELECT集合数组
int select_set_count = 0;     // SELECT集合数量
// 计算FIRST集
void compute_first_sets();
// 计算FOLLOW集
void compute_follow_sets();
// 计算SELECT集
void compute_select_sets();

// 解析产生式
void parse_production(const char* production_str, Production* production);
// 是否终结符
int is_terminal(char* symbol);
// 计算FIRST集
void compute_first_set(char* symbol, SymbolSet* set);
// 计算FOLLOW集
void compute_follow_set(char* symbol, SymbolSet* follow_set, SymbolSet* prev_set);
// 计算SELECT集
void compute_select_set(SelectSet* select_set, SymbolSet* first_set, SymbolSet* follow_set, Production* productions);
// 辅助函数：打印集合
void print_sets(SymbolSet* sets, int count);
// 辅助函数：合并集合
void merge_sets(SymbolSet* destination, SymbolSet* source);
// 辅助函数：把符号加入到集合中
void add_symbol_to_set(SymbolSet* set, const char* symbol);
// 辅助函数：查看符号是否在集合里
int is_symbol_in_set(SymbolSet* set, const char* symbol);
// 辅助函数：从集合中移除符号
void remove_symbol_from_set(SymbolSet* set, char* symbol);
// 去除字符串首尾空格, 会 malloc 一块新内存，用完需释放
extern char* trimSpaces(char* str);

const char* G_S[] = {
    "S → if E then S | while E do S | begin L end | A'",
    "L → S L'",
    "L' → ; S L' | ε",
    "A' → V = E", 
    "E → T E'",
    "E' → A T E' | ε",
    "T → F T'",
    "T' → M F T' | ε",
    "F → ( E ) | i",
    "A → + | -",
    "M → * | /",
    "V → i"

};
int main() {
    // TODO: 初始化产生式 productions 和 production_count
    // ...

    for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        parse_production(G_S[i], &productions[i]);
        production_count++;
    }

    // 打印测试
   /* for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        printf("LHS: %s\n", productions[i].lhs);
        for (int j = 0; j < productions[i].rhs_len; j++) {
            printf("RHS %d: %s\n", j, productions[i].rhs[j]);
        }
        printf("\n");
    }*/
    compute_first_sets();
    compute_follow_sets();
 
    // 打印结果
    printf("FIRST集合:\n");
    print_sets(first_sets, production_count);

    printf("\nFOLLOW集合:\n");
    print_sets(follow_sets, production_count);

    compute_select_sets();
    printf("\nSELECT集合:\n");
    for (int i = 0; i < select_set_count; i++) {
        printf("%s " RIGHT_ARROW " %s :", select_sets[i].p->lhs, select_sets[i].p->rhs);
        printf("{ \033[33m");
        for (int j = 0; j < select_sets[i].symset->size; j++) {
            printf("%s, ", select_sets[i].symset->symbols[j]);
        }
        printf("\033[0m }\n");
    }

    return 0;
}

void compute_select_sets() {
    compute_select_set(select_sets, first_sets, follow_sets, productions);
}

void init_select_set(SelectSet* select_set, char* lhs, char* rhs) {
    select_set->p = (Production*)malloc(sizeof(Production));
    MALLOC_NULL_EXIT(select_set->p);

    select_set->p->lhs = (char*)malloc(sizeof(char) * (strlen(lhs) + 1));
    MALLOC_NULL_EXIT(select_set->p->lhs);
    strcpy(select_set->p->lhs, lhs);

    select_set->p->rhs = (char*)malloc(sizeof(char) * (strlen(rhs) + 1));
    MALLOC_NULL_EXIT(select_set->p->rhs);
    strcpy(select_set->p->rhs, rhs);

    select_set->symset = (SymbolSet*)malloc(sizeof(SymbolSet));
    MALLOC_NULL_EXIT(select_set->symset);
    select_set->symset->capacity = select_set->symset->size = 0;
    select_set->symset->symbols = NULL;
    select_set->p->rhs_symbols = NULL;
}

void compute_select_set(SelectSet* select_sets, SymbolSet* first_sets, SymbolSet* follow_sets, Production* productions) {
    for (int i = 0; i < production_count; i++) {
        Production P = productions[i];
        for (int j = 0; j < P.rhs_len; j++) {
            
            char* rhs = (char*)malloc(sizeof(char) * (strlen(P.rhs_symbols[j]) + 1));
            MALLOC_NULL_EXIT(rhs);
            strcpy(rhs, P.rhs_symbols[j]);  // 复制字符串，避免修改原始字符串

            SelectSet select_set = { 0 };
            init_select_set(&select_set, P.lhs, rhs);

            char* rhs_symbol = strtok(rhs, " "); // 以空格分割 
            int p = 0;
            while (rhs_symbol != NULL) {
                rhs_symbol = trimSpaces(rhs_symbol);
                if (!is_terminal(rhs_symbol)) {
                    int k = 0;
                    for (; k < production_count; k++) {
                        if (strcmp(productions[k].lhs, rhs_symbol) == 0) {
                            break;
                        }
                    }
                    merge_sets(select_set.symset, &first_sets[k]);
                    if (!is_symbol_in_set(&first_sets[k], NIL)) {
                        free(rhs_symbol);
                        break;
                    }
                }
                else {
                    add_symbol_to_set(select_set.symset, rhs_symbol);
                    free(rhs_symbol);
                    break;
                } // else
                free(rhs_symbol);
                p++;
            } // while (rhs_symbol != NULL)
            
            if (is_symbol_in_set(select_set.symset, NIL) || strcmp(rhs, NIL) == 0) {
                int k = 0;
                for (; k < production_count; k++) {
                    if (strcmp(productions[k].lhs, P.lhs) == 0) {
                        break;
                    }
                }
                merge_sets(select_set.symset, &follow_sets[k]);
                remove_symbol_from_set(select_set.symset, NIL);
            }
            select_sets[select_set_count++] = select_set;

        } // for( j )
    } // for ( i )
}
void compute_first_sets() {
    for (int i = 0; i < production_count; i++) {
        compute_first_set(productions[i].lhs, &first_sets[i]);
    }
}

// 定义 compute_first_sets, compute_follow_sets, print_sets 等函数
// ...
void compute_first_set(char* symbol, SymbolSet* first_set) {
    if (is_terminal(symbol)) {
        add_symbol_to_set(first_set, symbol);
    }
    else {
        for (int i = 0; i < production_count; i++) {
            // 寻找左部匹配非终结符
            Production P = productions[i];
            if (strcmp(P.lhs, symbol) == 0) {

                for (int j = 0; j < P.rhs_len; j++) {
                    char* temp = (char*)malloc(sizeof(char) * (strlen(P.rhs_symbols[j]) + 1));
                    MALLOC_NULL_EXIT(temp);
                    strcpy(temp, P.rhs_symbols[j]);  // 复制字符串，避免修改原始字符串
                    char* rhs_symbol = strtok(temp, " "); // 以空格分割 
                    
                    int p = 0;
                    while (rhs_symbol != NULL) {
                        rhs_symbol = trimSpaces(rhs_symbol);
                        if (strcmp(rhs_symbol, NIL) == 0) {
                            add_symbol_to_set(first_set, NIL);
                            free(rhs_symbol);
                        }
                        else {
                            compute_first_set(rhs_symbol, first_set);
                            free(rhs_symbol);
                            if (!is_symbol_in_set(first_set, NIL)) {
                                break;
                            }
                        }
                        rhs_symbol = strtok(NULL, " "); // 下一符号
                        p++;
                    }
                    free(temp); // 用完记得释放
                    
                }
            }
        }
    }
}


void compute_follow_sets() {
    for (int i = 0; i < production_count; i++) {
        SymbolSet prev = { 0 };
        compute_follow_set(productions[i].lhs, &follow_sets[i], &prev);
    }
    
}

void compute_follow_set(char* symbol, SymbolSet* follow_set, SymbolSet* prev_set) {
    // 如果 symbol 是起始符号，加入 '$' 到 FOLLOW 集合
    if (strcmp(symbol, G_S_START) == 0) {
        add_symbol_to_set(follow_set, "$");
    }
    for (int i = 0; i < production_count; i++) { 
        Production P = productions[i];
        for (int j = 0; j < P.rhs_len; j++) {
            char* temp = (char*)malloc(sizeof(char) * (strlen(P.rhs_symbols[j]) + 1));
            MALLOC_NULL_EXIT(temp);
            strcpy(temp, P.rhs_symbols[j]);  // 复制字符串，避免修改原始字符串
            char* rhs_symbol = strtok(temp, " "); // 以空格分割 
            int p = 0;
            while (rhs_symbol != NULL) {
                rhs_symbol = trimSpaces(rhs_symbol);
   
                if (strcmp(rhs_symbol, symbol) == 0) { // P的右侧包含symbol, i + 1 < P 的右侧长度
                    char* next_rhs_symbol = strtok(NULL, " "); // 取下一条
                    if (next_rhs_symbol != NULL) { // 存在next_symbol
                        next_rhs_symbol = trimSpaces(next_rhs_symbol);
                        SymbolSet first_of_next = { 0 };
                        compute_first_set(next_rhs_symbol, &first_of_next);
                        merge_sets(follow_set, &first_of_next);


                        if (is_symbol_in_set(&first_of_next, NIL) && !is_symbol_in_set(prev_set, P.lhs)) {
                            SymbolSet new_prev_set = { 0 };
                            add_symbol_to_set(&new_prev_set, rhs_symbol);
                            merge_sets(&new_prev_set, prev_set);
                            compute_follow_set(P.lhs, follow_set, &new_prev_set);
                        }
                        free(next_rhs_symbol);
                    }
                    else if (!is_symbol_in_set(prev_set, P.lhs)) {
                        // 将 FOLLOW(P 的左侧) 加入 FOLLOW(symbol)
                        SymbolSet new_prev_set = { 0 };
                        add_symbol_to_set(&new_prev_set, rhs_symbol);
                        merge_sets(&new_prev_set, prev_set);
                        compute_follow_set(P.lhs, follow_set, &new_prev_set);

                    }
                    
                } // if (strcmp(rhs_symbol, symbol) == 0)

                free(rhs_symbol);
                rhs_symbol = strtok(NULL, " ");
                
                p++;
            } // while (rhs_symbol != NULL)

            free(temp); // 用完释放
        }
    }

    if (is_symbol_in_set(follow_set, NIL)) {
        remove_symbol_from_set(follow_set, NIL);
    }
}



void print_sets(SymbolSet* sets, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s: { ", productions[i].lhs);
        for (int j = 0; j < sets[i].size; j++) {
            printf("%s, ", sets[i].symbols[j]);
        }
        printf("}\n");
    }
}

int is_terminal(char* symbol) {
    // TODO: 判断是否为终结符
    // 不是大写字母就是终结符
    return (!strcmp(symbol, NIL)) || !(isalpha(symbol[0]) && isupper(symbol[0]));
}

// 辅助函数：合并集合
void merge_sets(SymbolSet* destination, SymbolSet* source) {
    for (int i = 0; i < source->size; i++) {
        add_symbol_to_set(destination, source->symbols[i]);
    }
}


// 辅助函数：从集合中移除符号
void remove_symbol_from_set(SymbolSet* set, char* symbol) {
    for (int i = 0; i < set->size; i++) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            // 移动最后一个元素到当前位置并减少集合大小
            set->symbols[i] = set->symbols[set->size - 1];
            set->size--;
            break;
        }
    }
}
// 辅助函数定义
void add_symbol_to_set(SymbolSet* set, const char* symbol) {
    // 检查符号是否已存在
    if (is_symbol_in_set(set, symbol)) {
        return; // 符号已存在，不添加
    }

    // 检查容量是否足够，如果不足则扩展
    if (set->size == set->capacity) {
        if (set->capacity == 0) {
            set->capacity = 1;
        }
        else {
            set->capacity *= 2; // 加倍容量
        }
        set->symbols = (char**)realloc(set->symbols, set->capacity * sizeof(char*));
    }

    // 添加新符号
    set->symbols[set->size] = _strdup(symbol); // 分配内存并复制符号 str duplicate
    set->size++;
}

int is_symbol_in_set(SymbolSet* set, const char* symbol) {
    for (int i = 0; i < set->size; i++) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            return 1; // 找到符号，返回 1
        }
    }
    return 0; // 未找到符号，返回 0
}

void parse_production(const char* production_str, Production* production) {
    char* temp = (char*)malloc(sizeof(char) * (strlen(production_str) + 1));
    MALLOC_NULL_EXIT(temp);
    strcpy(temp, production_str);  // 复制字符串，避免修改原始字符串

    // 分割左侧和右侧
    {
        char* lhs = strtok(temp, RIGHT_ARROW);
        lhs = trimSpaces(lhs);
        production->lhs = (char*)malloc(sizeof(char) * (strlen(lhs) + 1));
        MALLOC_NULL_EXIT(production->lhs);
        strcpy(production->lhs, lhs);
        free(lhs);
    }
    
    // 处理右侧
    // 箭头右边的字符串
    char* rhs = strtok(NULL, RIGHT_ARROW);
    production->rhs = rhs;

    rhs = trimSpaces(rhs); // copy of rhs

    char* token = strtok(rhs, "|");
    
    int i = 0;
    production->rhs_symbols = (char**)malloc(sizeof(char*) * MAX_SYMBOLS);
    MALLOC_NULL_EXIT(production->rhs_symbols);
    while (token != NULL) {
        //printf("token %d: %s\n", i + 1, token);
        token = trimSpaces(token);
        production->rhs_symbols[i] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
        MALLOC_NULL_EXIT(production->rhs_symbols[i]);
        strcpy(production->rhs_symbols[i], token);
        free(token);
        token = strtok(NULL, "|");
        i++;
    }
    free(rhs);
    production->rhs_len = i;
    free(temp);
}

