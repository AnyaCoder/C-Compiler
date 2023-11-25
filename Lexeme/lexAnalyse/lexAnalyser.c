#include "common_headers.h"
#include "lexAnalyzer.h"


const char* kept_words[] = {
    "void", "char", "short", "int", "float", "double", "long", "auto", "unsigned", "signed",
    "enum","typedef", "union", "struct",
    "if", "else","switch", "case", "default", "return", "goto",
    "for", "do", "while", "break", "continue",
    "volatile", "const", "then", "begin", "end",
    NULL
};
// Token类型的枚举
typedef enum {
    MACROS = 1,
    IDENTIFIER,
    UNSIGNED_INTEGER,
    SINGLE_CHAR_DELIMITER,
    DOUBLE_CHAR_DELIMITER,
    RESERVED_WORD,
    ERROR,
    COMMENT,
    OK
} TokenType;

const char* token_type_str(TokenType _ty) {
    switch (_ty)
    {
    case MACROS:
        return "宏定义";
    case IDENTIFIER:
        return "标识符";
    case UNSIGNED_INTEGER:
        return "无符号整数";
    case SINGLE_CHAR_DELIMITER:
        return "单字符分隔符";
    case DOUBLE_CHAR_DELIMITER:
        return "双字符分隔符";
    case RESERVED_WORD:
        return "保留字/关键字";
    case COMMENT:
        return "注释";
    case ERROR:
        return "ERROR";
    default:
        return "OK";
    }
}
int global_line = 1;
int in_comment = 0;
int in_comma = 0;
// Token结构体
typedef struct {
    TokenType type; // 类型
    int idx; // 编号
    int line; // 行号
    unsigned char lexeme[33]; // 最长标识符长度+1
} Token;

#define LEX_ERROR(msg)  {strcpy(token.lexeme, msg); \
                        while (!isspace(c)) { \
                            c = NEXT_POS; \
                        } \
                        token.idx = 0; \
                        token.type = ERROR;}

// 词法分析函数
Token lex(unsigned char** input) {
    Token token;
    unsigned char c = (unsigned char)CUR_POS;
    if (c == 0) {
        strcpy(token.lexeme, "Finished Lexeme Analysis!");
        token.type = OK;
        token.line = global_line;
        return token;
    }
    // 跳过空白字符
    while (isspace(c)) {
        if (c == '\n') {
            if (in_comment == 1) {
                in_comment = 0;
            }
            if (in_comma == 1) {
                in_comma = 0;
                token.type = ERROR;
                token.line = global_line++;
                LEX_ERROR("Err: Invalid comma closure!");
                c = NEXT_POS;
                return token;
            }
            ++global_line;
        }
        c = NEXT_POS;
    }
    token.type = OK;
    token.line = global_line;
    // 根据字符分类进行识别
    if (isalpha(c) || c == '_') {
        // 标识符
        int i = 0;
        while (isalnum(c) || c == '_') {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
            if (i == MAX_IDEN_LEN && !isspace(c)) {
                LEX_ERROR("Err: Too long identifier!")
                    while (!isspace(c)) {
                        c = NEXT_POS; // skip
                    }
                return token;
            }
        }
        token.lexeme[i] = '\0';
        
        if (isIdentifier(token.lexeme)) {
            int j = 0;
            for (j = 0; kept_words[j]; ++j) {
                if (!strcmp(token.lexeme, kept_words[j])) {
                    token.type = RESERVED_WORD;
                    token.idx = j + 1;
                    break;
                }
            }
            if (in_comment) {
                token.type = COMMENT;
                token.idx = (in_comment == 1 ? 77 : 78);
            }
            else if (token.type != RESERVED_WORD) {
                token.type = IDENTIFIER;
                token.idx = 32;
            }
        }
        else {
            LEX_ERROR("Err: Invalid identifier!");
        }
        
    }
    else if (isdigit(c)) {
        int i = 0;
        while (isalnum(c) || c == '+' || c == '-' || toupper(c) == 'E') {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
        }
        token.lexeme[i] = '\0';
        if (in_comment) {
            token.type = COMMENT;
            token.idx = 33;
        }
        else if (isUnsignedNumber(token.lexeme)) {
            while (isdigit(c) || c == '+' || c == '-' || toupper(c) == 'E' || c == '.') {
                if (i < MAX_IDEN_LEN) {
                    token.lexeme[i++] = c;
                }
                c = NEXT_POS;
            }
            token.lexeme[i] = '\0';
            token.type = UNSIGNED_INTEGER;
            token.idx = 33;
        }
        else {
            LEX_ERROR("Err: Invalid unsigned number!");
            return token;
        }
    }
    else if (isSingleSeparator(c)) {
        int i = 0;
        char* single_pos = ((*input) + 1);
        while (isSingleSeparator(c) && i < 2) {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
        }
        token.lexeme[i] = '\0';
        //printf("%s\n", token.lexeme);
        if (isComment(token.lexeme)) {
            //printf("%s\n", token.lexeme);
            if (in_comment != 2 && !strcmp(token.lexeme, "//")) {
                in_comment = 1;
            }
            else if (!strcmp(token.lexeme, "/*")) {
                if (in_comment == 0) {
                    in_comment = 2;
                }
            }
            else if (!strcmp(token.lexeme, "*/")) {
                if (in_comment == 0) {
                    LEX_ERROR("Err:Invalid multiline comment!");
                }
                else if (in_comment == 2) {
                    in_comment = 0;
                }   
            }
            token.type = COMMENT;
            token.idx = (in_comment == 1 ? 77 : 78);
        }
        else if (in_comment) {
            token.type = COMMENT;
            token.idx = (in_comment == 1 ? 77 : 78);
        }
        else if (isDoubleSeparator(token.lexeme)) {
            token.type = DOUBLE_CHAR_DELIMITER;
            token.idx = get_idx_double(token.lexeme);
        }
        else {
            token.type = SINGLE_CHAR_DELIMITER;
            char cc = token.lexeme[0];
            token.idx = get_idx_single(cc);
            char tmp[] = { cc , '\0' };
            strcpy(token.lexeme, tmp);
            *input = single_pos;
            if (cc == '"') {
                in_comma = !in_comma;
            }
        }
    }
    else if (c == '#') {
        int i = 0;
        token.lexeme[i++] = c;
        c = NEXT_POS;
        while (isalnum(c) || c == '_') {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
        }
        token.lexeme[i] = '\0';
        if (in_comment) {
            token.type = COMMENT;
            token.idx = 77;
        }
        else if (isMacro(token.lexeme)) {
            token.type = MACROS;
            token.idx = get_idx_single(token.lexeme[0]);
        }
        else {
            LEX_ERROR("Err: Invalid macro!");
        }
    }
    else {
        if (in_comment) {
            token.type = COMMENT;
            token.idx = 0;
            char tmp[] = { c, '\0' };
            strcpy(token.lexeme, (const char*)tmp);

        }
        else {
            LEX_ERROR("Err: Invaild Identifier!");
        }
        

        c = NEXT_POS;
    }


    return token;
}

void init_lex() {
    init_iden();
    init_unsigned();
    init_double_sep();
    init_macro();
    init_comment();
}
int main() {
    init_lex();

    unsigned char in_file_name[256];
    unsigned char out_file_name[256];
    printf("Enter the in_file_name of the C file: ");
    scanf("%255s", in_file_name);
    printf("Enter the out_file_name of the C file: ");
    scanf("%255s", out_file_name);
    FILE* in_file = fopen(in_file_name, "r");
    if (!in_file) {
        perror("Error opening file");
        return 1;
    }

    fseek(in_file, 0, SEEK_END);
    long fsize = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    unsigned char* input = (unsigned char*)malloc(sizeof(unsigned char) * (fsize + 1));
    if (!input) {
        perror("Failed to allocate memory");
        fclose(in_file);
        return 1;
    }

    size_t bytesRead = fread(input, 1, fsize, in_file);
    //printf("%d %d\n", (int)bytesRead, (int)fsize);
    input[bytesRead] = '\0';
    if (bytesRead > fsize) {
        perror("Failed to read the complete file");
        free(input);
        fclose(in_file);
        return 1;
    }
    fclose(in_file);

    Token token;
    char* inputPtr = input;

    FILE* out_file;
    
    out_file = fopen(out_file_name, "w");
    if (!out_file) {
        perror("Error opening out_file");
        return 1;
    }
    while ((*inputPtr) != '\0' && (token = lex(&inputPtr)).type != OK){
        printf("Type: (%d)%15s, Lexeme:%20s , Line: %d\n", token.idx, token_type_str(token.type), token.lexeme, token.line);
        if (token.type != COMMENT) {
            fprintf(out_file, "%d, %d, %s\n", token.line, token.idx, token.lexeme);
        }
    }
    fclose(out_file);
    printf("词法分析程序已完成，已经将结果写入：%s\n", out_file_name);
    free(input);

    return 0;
}
