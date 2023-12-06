#include "commons.h"

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

// ����ȫ�ֱ���
Production productions[100];  // ����ʽ����
int production_count = 0;     // ����ʽ����
SymbolSet first_sets[100];    // FIRST��������
SymbolSet follow_sets[100];   // FOLLOW��������
SelectSet select_sets[100];   // SELECT��������
int select_set_count = 0;     // SELECT��������
// ����FIRST��
void compute_first_sets();
// ����FOLLOW��
void compute_follow_sets();
// ����SELECT��
void compute_select_sets();

// ��������ʽ
void parse_production(const char* production_str, Production* production);
// �Ƿ��ս��
int is_terminal(char* symbol);
// ����FIRST��
void compute_first_set(char* symbol, SymbolSet* set);
// ����FOLLOW��
void compute_follow_set(char* symbol, SymbolSet* follow_set, SymbolSet* prev_set);
// ����SELECT��
void compute_select_set(SelectSet* select_set, SymbolSet* first_set, SymbolSet* follow_set, Production* productions);
// ������������ӡ����
void print_sets(SymbolSet* sets, int count);
// �����������ϲ�����
void merge_sets(SymbolSet* destination, SymbolSet* source);
// �����������ѷ��ż��뵽������
void add_symbol_to_set(SymbolSet* set, const char* symbol);
// �����������鿴�����Ƿ��ڼ�����
int is_symbol_in_set(SymbolSet* set, const char* symbol);
// �����������Ӽ������Ƴ�����
void remove_symbol_from_set(SymbolSet* set, char* symbol);
// ȥ���ַ�����β�ո�, �� malloc һ�����ڴ棬�������ͷ�
extern char* trimSpaces(char* str);

const char* G_S[] = {
    "S �� if E then S | while E do S | begin L end | A'",
    "L �� S L'",
    "L' �� ; S L' | ��",
    "A' �� V = E", 
    "E �� T E'",
    "E' �� A T E' | ��",
    "T �� F T'",
    "T' �� M F T' | ��",
    "F �� ( E ) | i",
    "A �� + | -",
    "M �� * | /",
    "V �� i"

};
int main() {
    // TODO: ��ʼ������ʽ productions �� production_count
    // ...

    for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        parse_production(G_S[i], &productions[i]);
        production_count++;
    }

    // ��ӡ����
   /* for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        printf("LHS: %s\n", productions[i].lhs);
        for (int j = 0; j < productions[i].rhs_len; j++) {
            printf("RHS %d: %s\n", j, productions[i].rhs[j]);
        }
        printf("\n");
    }*/
    compute_first_sets();
    compute_follow_sets();
 
    // ��ӡ���
    printf("FIRST����:\n");
    print_sets(first_sets, production_count);

    printf("\nFOLLOW����:\n");
    print_sets(follow_sets, production_count);

    compute_select_sets();
    printf("\nSELECT����:\n");
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
            strcpy(rhs, P.rhs_symbols[j]);  // �����ַ����������޸�ԭʼ�ַ���

            SelectSet select_set = { 0 };
            init_select_set(&select_set, P.lhs, rhs);

            char* rhs_symbol = strtok(rhs, " "); // �Կո�ָ� 
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

// ���� compute_first_sets, compute_follow_sets, print_sets �Ⱥ���
// ...
void compute_first_set(char* symbol, SymbolSet* first_set) {
    if (is_terminal(symbol)) {
        add_symbol_to_set(first_set, symbol);
    }
    else {
        for (int i = 0; i < production_count; i++) {
            // Ѱ����ƥ����ս��
            Production P = productions[i];
            if (strcmp(P.lhs, symbol) == 0) {

                for (int j = 0; j < P.rhs_len; j++) {
                    char* temp = (char*)malloc(sizeof(char) * (strlen(P.rhs_symbols[j]) + 1));
                    MALLOC_NULL_EXIT(temp);
                    strcpy(temp, P.rhs_symbols[j]);  // �����ַ����������޸�ԭʼ�ַ���
                    char* rhs_symbol = strtok(temp, " "); // �Կո�ָ� 
                    
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
                        rhs_symbol = strtok(NULL, " "); // ��һ����
                        p++;
                    }
                    free(temp); // ����ǵ��ͷ�
                    
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
    // ��� symbol ����ʼ���ţ����� '$' �� FOLLOW ����
    if (strcmp(symbol, G_S_START) == 0) {
        add_symbol_to_set(follow_set, "$");
    }
    for (int i = 0; i < production_count; i++) { 
        Production P = productions[i];
        for (int j = 0; j < P.rhs_len; j++) {
            char* temp = (char*)malloc(sizeof(char) * (strlen(P.rhs_symbols[j]) + 1));
            MALLOC_NULL_EXIT(temp);
            strcpy(temp, P.rhs_symbols[j]);  // �����ַ����������޸�ԭʼ�ַ���
            char* rhs_symbol = strtok(temp, " "); // �Կո�ָ� 
            int p = 0;
            while (rhs_symbol != NULL) {
                rhs_symbol = trimSpaces(rhs_symbol);
   
                if (strcmp(rhs_symbol, symbol) == 0) { // P���Ҳ����symbol, i + 1 < P ���Ҳ೤��
                    char* next_rhs_symbol = strtok(NULL, " "); // ȡ��һ��
                    if (next_rhs_symbol != NULL) { // ����next_symbol
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
                        // �� FOLLOW(P �����) ���� FOLLOW(symbol)
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

            free(temp); // �����ͷ�
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
    // TODO: �ж��Ƿ�Ϊ�ս��
    // ���Ǵ�д��ĸ�����ս��
    return (!strcmp(symbol, NIL)) || !(isalpha(symbol[0]) && isupper(symbol[0]));
}

// �����������ϲ�����
void merge_sets(SymbolSet* destination, SymbolSet* source) {
    for (int i = 0; i < source->size; i++) {
        add_symbol_to_set(destination, source->symbols[i]);
    }
}


// �����������Ӽ������Ƴ�����
void remove_symbol_from_set(SymbolSet* set, char* symbol) {
    for (int i = 0; i < set->size; i++) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            // �ƶ����һ��Ԫ�ص���ǰλ�ò����ټ��ϴ�С
            set->symbols[i] = set->symbols[set->size - 1];
            set->size--;
            break;
        }
    }
}
// ������������
void add_symbol_to_set(SymbolSet* set, const char* symbol) {
    // �������Ƿ��Ѵ���
    if (is_symbol_in_set(set, symbol)) {
        return; // �����Ѵ��ڣ������
    }

    // ��������Ƿ��㹻�������������չ
    if (set->size == set->capacity) {
        if (set->capacity == 0) {
            set->capacity = 1;
        }
        else {
            set->capacity *= 2; // �ӱ�����
        }
        set->symbols = (char**)realloc(set->symbols, set->capacity * sizeof(char*));
    }

    // ����·���
    set->symbols[set->size] = _strdup(symbol); // �����ڴ沢���Ʒ��� str duplicate
    set->size++;
}

int is_symbol_in_set(SymbolSet* set, const char* symbol) {
    for (int i = 0; i < set->size; i++) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            return 1; // �ҵ����ţ����� 1
        }
    }
    return 0; // δ�ҵ����ţ����� 0
}

void parse_production(const char* production_str, Production* production) {
    char* temp = (char*)malloc(sizeof(char) * (strlen(production_str) + 1));
    MALLOC_NULL_EXIT(temp);
    strcpy(temp, production_str);  // �����ַ����������޸�ԭʼ�ַ���

    // �ָ������Ҳ�
    {
        char* lhs = strtok(temp, RIGHT_ARROW);
        lhs = trimSpaces(lhs);
        production->lhs = (char*)malloc(sizeof(char) * (strlen(lhs) + 1));
        MALLOC_NULL_EXIT(production->lhs);
        strcpy(production->lhs, lhs);
        free(lhs);
    }
    
    // �����Ҳ�
    // ��ͷ�ұߵ��ַ���
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

