#include "common_headers.h"
#include "lexAnalyzer.h"


/*

<标识符> → [a-zA-Z][a-zA-Z0-9]{0,31}
<无符号整数> → [0-9]+
<单字符分界符> → [+\-;,\(\){}/|\*|=|>|<]
<双字符分界符> → (<<|>>|\+=|-=|\*=|/=|&&| \|\| |!=|==|)
<小于> → <
<等于> → =
<大于> → >
<斜竖> → /
<感叹号> → !
<保留字> → (void|int|float|double|if|else|for|do|while|...)


*/


const char* kept_words[] = {
    "void", "char", "short", "int", "float", "double", "long", "auto", "unsigned", "signed",
    "enum","typedef", "union", "struct",
    "if", "else","switch", "case", "default", "return", "goto",
    "for", "do", "while", "break", "continue",
    "volatile", "const", "register", "extern", "static",
    "sizeof", NULL
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

// Token结构体
typedef struct {
    TokenType type;
    int line;
    unsigned char lexeme[33]; // 最长标识符长度+1
} Token;

#define LEX_ERROR(msg)  {strcpy(token.lexeme, msg); \
                        while (!isspace(c)) { \
                            c = NEXT_POS; \
                        } \
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
        }
        token.lexeme[i] = '\0';
        if (isIdentifier(token.lexeme)) {
            int j = 0;
            for (j = 0; kept_words[j]; ++j) {
                if (!strcmp(token.lexeme, kept_words[j])) {
                    token.type = RESERVED_WORD;
                    break;
                }
            }
            if (token.type != RESERVED_WORD) {
                token.type = IDENTIFIER;
            }
        }
        else {
            LEX_ERROR("Err: Not an identifier!");
        }
        
    }
    else if (isdigit(c)) {
        int i = 0;
        while (isdigit(c) || c == '+' || c == '-' || toupper(c) == 'E') {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
        }
        token.lexeme[i] = '\0';
        if (isUnsignedNumber(token.lexeme)) {
            while (isdigit(c) || c == '+' || c == '-' || toupper(c) == 'E') {
                if (i < MAX_IDEN_LEN) {
                    token.lexeme[i++] = c;
                }
                c = NEXT_POS;
            }
            token.lexeme[i] = '\0';
            token.type = UNSIGNED_INTEGER;
        }
        else {
            LEX_ERROR("Err: Not an unsigned number!");
            return token;
        }
    }
    else if (isSingleSeparator(c)) {
        int i = 0;
        char* single_pos = ((*input) + 1);
        while (isSingleSeparator(c)) {
            if (i < MAX_IDEN_LEN) {
                token.lexeme[i++] = c;
            }
            c = NEXT_POS;
        }
        token.lexeme[i] = '\0';
        if (i == 1) {
            token.type = SINGLE_CHAR_DELIMITER;
        }
        else if (isDoubleSeparator(token.lexeme)) {
            token.type = DOUBLE_CHAR_DELIMITER;
        }
        else if (isComment(token.lexeme)) {
            token.type = COMMENT;
        }
        else {
            token.type = SINGLE_CHAR_DELIMITER;
            char s[2] = { token.lexeme[0], '\0' };
            strcpy(token.lexeme, s);
            *input = single_pos;
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
        if (isMacro(token.lexeme)) {
            token.type = MACROS;
        }
    }
    else {
        token.type = COMMENT;
        char tmp[] = { c, '\0' };
        strcpy(token.lexeme, (const char*)tmp);
        //LEX_ERROR("unknown type");

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

    unsigned char filename[256];
    printf("Enter the filename of the C file: ");
    scanf("%255s", filename); // read filename from user

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* input = (unsigned char*)malloc(sizeof(unsigned char) * (fsize + 1));
    if (!input) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    
    size_t bytesRead = fread(input, 1, fsize, file);
    //printf("%d %d\n", (int)bytesRead, (int)fsize);
    input[bytesRead] = '\0';
    if (bytesRead > fsize) {
        perror("Failed to read the complete file");
        free(input);
        fclose(file);
        return 1;
    }
    fclose(file);

    printf("%s\n", input);

    Token token;
    char* inputPtr = input;

    while ((*inputPtr) != '\0' && (token = lex(&inputPtr)).type != ERROR) {
        printf("Type: (%d)%15s, Lexeme:%20s , Line: %d\n", token.type, token_type_str(token.type), token.lexeme, token.line);
    }

    printf("%s", inputPtr);
    free(input);

    return 0;
}
