#include "commons.h"

/*  ����ȫ�ֱ���  */
extern Production productions[100];  // ����ʽ����
extern int production_count;     // ����ʽ����
extern SymbolSet first_sets[100];    // FIRST��������
extern SymbolSet follow_sets[100];   // FOLLOW��������
extern SelectSet select_sets[100];   // SELECT��������
extern int select_set_count;     // SELECT��������
extern TrieNode* root;

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
    select_set->p->rhs_len = 0;

    select_set->symset = (SymbolSet*)malloc(sizeof(SymbolSet));
    MALLOC_NULL_EXIT(select_set->symset);

    select_set->symset->capacity = select_set->symset->size = 0;
    select_set->symset->symbols = NULL;

    select_set->p->rhs_symbols = (char**)malloc(sizeof(char*) * MAX_SYMBOLS); // ������MAX_SYMBOLS��
    MALLOC_NULL_EXIT(select_set->p->rhs_symbols);
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
            while (rhs_symbol != NULL) {
                rhs_symbol = trimSpaces(rhs_symbol);
                select_set.p->rhs_symbols[select_set.p->rhs_len] \
                    = (char*)malloc(sizeof(char) * (strlen(rhs_symbol) + 1));
                MALLOC_NULL_EXIT(select_set.p->rhs_symbols[select_set.p->rhs_len]);
                strcpy(select_set.p->rhs_symbols[select_set.p->rhs_len], rhs_symbol);
                select_set.p->rhs_len++;
                free(rhs_symbol);
                rhs_symbol = strtok(NULL, " ");
            }

            strcpy(rhs, P.rhs_symbols[j]);
            rhs_symbol = strtok(rhs, " ");
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
                rhs_symbol = strtok(NULL, " ");
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
            char* saveptr[2];
            char* rhs_symbol = STRTOK(temp, " ", &saveptr[0]); // �Կո�ָ� 
            int p = 0;
            while (rhs_symbol != NULL) {
                rhs_symbol = trimSpaces(rhs_symbol);
   
                if (strcmp(rhs_symbol, symbol) == 0) { // P���Ҳ����symbol, i + 1 < P ���Ҳ೤��
                    char* next_rhs_symbol = STRTOK(NULL, " ", &saveptr[0]); // ȡ��һ��
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
                rhs_symbol = STRTOK(NULL, " ", &saveptr[0]);
                
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

void parse_production_lhs(const char* production_str, Production* production) {
    char* temp = (char*)malloc(sizeof(char) * (strlen(production_str) + 1));
    MALLOC_NULL_EXIT(temp);
    strcpy(temp, production_str);  // �����ַ����������޸�ԭʼ�ַ���

    char* lhs = strtok(temp, RIGHT_ARROW);
    lhs = trimSpaces(lhs);
    production->lhs = (char*)malloc(sizeof(char) * (strlen(lhs) + 1));
    MALLOC_NULL_EXIT(production->lhs);
    strcpy(production->lhs, lhs);
    printf("%s: Index= %d\n", lhs, insertAndGetIndex(root, lhs));
    free(lhs);
    
    free(temp);
}

void parse_production_rhs(const char* production_str, Production* production) {
    char* saveptr[3];
    char* temp = (char*)malloc(sizeof(char) * (strlen(production_str) + 1));
    MALLOC_NULL_EXIT(temp);
    strcpy(temp, production_str);  // �����ַ����������޸�ԭʼ�ַ���
    char* lhs = STRTOK(temp, RIGHT_ARROW, &saveptr[0]);
    char* rhs = STRTOK(NULL, RIGHT_ARROW, &saveptr[0]);
    production->rhs = rhs;

    rhs = trimSpaces(rhs); // copy of rhs
    char* token = STRTOK(rhs, "|", &saveptr[1]);
    int i = 0;
    production->rhs_symbols = (char**)malloc(sizeof(char*) * MAX_SYMBOLS);
    MALLOC_NULL_EXIT(production->rhs_symbols);
    while (token != NULL) {
        //printf("token %d: %s\n", i + 1, token);
        token = trimSpaces(token);
        //printf("%s: Index= %d\n", token, insertAndGetIndex(root, token));
        production->rhs_symbols[i] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
        MALLOC_NULL_EXIT(production->rhs_symbols[i]);
        strcpy(production->rhs_symbols[i], token);

        /************************/

        char* token_sep = STRTOK(token, " ", &saveptr[2]);
        while (token_sep != NULL) {
            token_sep = trimSpaces(token_sep);
            if (strcmp(token_sep, NIL) == 0) { // '��' not considered
                break;
            }
            printf("%s: Index= %d\n", token_sep, insertAndGetIndex(root, token_sep));
            free(token_sep);
            token_sep = STRTOK(NULL, " ", &saveptr[2]);
        }
        /************************/

        free(token);
        token = STRTOK(NULL, "|", &saveptr[1]);
        i++;
    }
    free(rhs);
    
    production->rhs_len = i;
}
