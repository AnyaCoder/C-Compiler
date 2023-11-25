#define CUR_POS **input // 当前字符指针
#define NEXT_POS *(++(*input))  // 指针后移，并且取后移的字符
#define MAX_IDEN_LEN 32  // 标识符最大长度

int isUnsignedNumber(const char* s); // 给定小字符串是否为无符号数
int isIdentifier(const char* s);  // 是否为合法的标识符/关键字
int isSingleSeparator(char c); // 是否为单字符分隔符
int isDoubleSeparator(const char* s); // 是否为双字符分隔符
int isComment(const char* s);   // 是否为注释
int isMacro(const char* s);   // 是否为宏

int get_idx_single(char c);  // 得到单字符分隔符的类型编号
int get_idx_double(const char* double_sep); // 得到双字符分隔符的类型编号

void init_iden(); // 初始化识别标识符DFA的状态转移矩阵
void init_unsigned(); // 初始化识别无符号数DFA的状态转移矩阵
void init_double_sep(); // 初始化识别双字符分隔符DFA的状态转移矩阵
void init_macro(); // 初始化识别宏的DFA的状态转移矩阵
void init_comment(); // 初始化识别注释的DFA的状态转移矩阵