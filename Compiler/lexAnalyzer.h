
#define CUR_POS **input
#define NEXT_POS *(++(*input))
#define MAX_IDEN_LEN 32
#define _bool char
#define _true 1
#define _false 0

int isUnsignedNumber(const char* s);
int isIdentifier(const char* s);
int isSingleSeparator(char c);
int isDoubleSeparator(const char* s);
int isComment(const char* s);
int isMacro(const char* s);

int get_idx_single(char c);
int get_idx_double(const char* double_sep);

void init_iden();
void init_unsigned();
void init_double_sep();
void init_macro();
void init_comment();