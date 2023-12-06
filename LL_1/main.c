#include "commons.h"

Token tokens[250]; // ��ǰ����Ĵʷ���Ԫ����
int currentToken = 0;   // ��ǰ����token��
int totalTokens = 0;    // ��token��

const char* str_TokenType[] = {
    "S", "L", "L'", "A'", "E", "E'", "T", "T'", "F", "A", "M", "V",
    "IF", "THEN", "WHILE", "DO", "BEGIN", "END", "SEMICOLON",
    "ASSIGN", "L_BRANKET", "R_BRANKET",  "VAR", "PLUS", "MINUS", "MUL", "DIV", "EOS",
    "UNKNOWN"
};

const char* str_Token[] = {
    "S", "L", "L'", "A'", "E", "E'", "T", "T'", "F", "A", "M", "V",
    "if", "then", "while", "do", "begin", "end",  ";",   
    "=", "(", ")", "i", "+", "-", "*", "/", "$",
    "UNKNOWN"
};
typedef struct TokenStack {
    Token tokens[250];
    int top;
} TokenStack;

TokenStack symbol_stack, analyse_stack; // ����ջ�� ����ջ

void push_stack(TokenStack* stack, Token token) { // ���費�ᱬջ
    stack->tokens[++(stack->top)] = token;
}
void print_analy_stack(TokenStack* stack) {
    printf("����ջ�����{ \033[33m");
    for (int i = stack->top; i >= 1; i--) {
        printf("%s, ", str_TokenType[stack->tokens[i].type]);
    }
    printf("\033[0m }\n");
}

void print_sym_stack(Token* tokens) {
    printf("����ջ�����{ \033[32m");
    for (int i = currentToken; i <= totalTokens; i++) {
        printf("%s, ", str_Token[tokens[i].type]);
    }
    printf("\033[0m }\n");
}
Token pop_stack(TokenStack* stack) { // ���費�ᵯ��ջ
    return stack->tokens[(stack->top)--];
}

Token top_stack(TokenStack* stack) {
    return stack->tokens[(stack->top)];
}
// ����ȫ�ֱ���
Production productions[100];  // ����ʽ����
int production_count = 0;     // ����ʽ����
SymbolSet first_sets[100];    // FIRST��������
SymbolSet follow_sets[100];   // FOLLOW��������
SelectSet select_sets[100];   // SELECT��������
int select_set_count = 0;     // SELECT��������

TrieNode* root;
TableEntry LL_1_table[40][40];

// �Ƿ��ս��
extern int is_terminal(char* symbol);
extern void print_sets(SymbolSet* sets, int count);
void idx_to_token(struct Token* pToken);
void LL_1();

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
    root = newTrieNode();
    for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        parse_production_lhs(G_S[i], &productions[i]);
        production_count++;
    }
    for (int i = 0; i < sizeof(G_S) / sizeof(G_S[0]); i++) {
        parse_production_rhs(G_S[i], &productions[i]);
    }
    printf("$: %d\n", insertAndGetIndex(root, "$"));
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
    printf("\nSELECT�����Լ�ͬ���ʷ���Ԫ����:\n");
    for (int i = 0; i < select_set_count; i++) {
        printf("%s " RIGHT_ARROW " %s :", select_sets[i].p->lhs, select_sets[i].p->rhs);
        printf("{ \033[32m");
        int lhs_index = insertAndGetIndex(root, select_sets[i].p->lhs);

        for (int j = 0; j < select_sets[i].symset->size; j++) {
            printf("%s, ", select_sets[i].symset->symbols[j]);
            int rhs_symbol_index = insertAndGetIndex(root, select_sets[i].symset->symbols[j]);
            LL_1_table[lhs_index][rhs_symbol_index].select = &select_sets[i];
        }
        printf("\033[0m }\t");
        printf("{ \033[33m");
        // ͬ���ʷ���Ԫ
        for (int j = 0; j < follow_sets[lhs_index].size; j++) {
            int rhs_symbol_index = insertAndGetIndex(root, follow_sets[lhs_index].symbols[j]);
            printf("%s, ", follow_sets[lhs_index].symbols[j]);
            LL_1_table[lhs_index][rhs_symbol_index] = (TableEntry){ .sync = 1};
        }
        printf("\033[0m }\n");
        
    }
    /***************** ��ʼ����� *************/
    printf("Made by zrz\n");
    puts("������ʷ������ı�·��: ");
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
    token.type = EOS;
    tokens[totalTokens] = token;
    fclose(in_file);
    LL_1();

    return 0;
}


void LL_1() {
    currentToken = 0;

    push_stack(&analyse_stack, (Token) { .type = EOS });
    push_stack(&analyse_stack, (Token) { .type = S });
    enum TokenType cur_type, top_type;
    while (currentToken <= totalTokens) {
        cur_type = tokens[currentToken].type;
        top_type = top_stack(&analyse_stack).type;
        print_sym_stack(tokens);
        print_analy_stack(&analyse_stack);

        if (top_type == EOS) {
            if (cur_type == EOS) {
                puts("Success!");
            }
            else {
                puts("There are redundant symbols in the source!");
            }
            
            break;
        }
        else if (cur_type == top_type) {
            currentToken++;
            printf("Pop (T): %s\n", str_TokenType[pop_stack(&analyse_stack).type]);
            continue;
        }
        SelectSet* select = LL_1_table[top_type][cur_type].select;
        int is_sync = LL_1_table[top_type][cur_type].sync;
        if (select != NULL) {
            printf("Pop (NT): %s\n", str_TokenType[pop_stack(&analyse_stack).type]);
            Production* P = select->p;
            for (int i = P->rhs_len - 1; i >= 0; i--) {
                int rhs_symbol_index = insertAndGetIndex(root, P->rhs_symbols[i]);
                push_stack(&analyse_stack, (Token) { .type = rhs_symbol_index });
                printf("Push : %s\n", str_TokenType[rhs_symbol_index]);
            }
        }
        else if (is_sync) {
            printf("Pop (Sync): %s\n", str_TokenType[pop_stack(&analyse_stack).type]);
        }
        else {
         
            printf("\033[31m Syntax Error! Found: %s, Expected: {", \
                str_Token[cur_type]);
            if (is_terminal(str_Token[top_type])) {
                printf("%s", str_Token[top_type]);
            } 
            else {
                for (int i = 0; i < select_set_count; i++) {
                    int lhs_index = insertAndGetIndex(root, select_sets[i].p->lhs);
                    if (lhs_index == top_type) {
                        for (int j = 0; j < select_sets[i].symset->size; j++) {
                            printf("%s, ", select_sets[i].symset->symbols[j]);
                        }
                    }
                }
               
            }
            printf(" } \033[0m\n");
            currentToken++;
        }
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
