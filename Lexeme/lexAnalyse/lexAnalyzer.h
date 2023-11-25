#define CUR_POS **input // ��ǰ�ַ�ָ��
#define NEXT_POS *(++(*input))  // ָ����ƣ�����ȡ���Ƶ��ַ�
#define MAX_IDEN_LEN 32  // ��ʶ����󳤶�

int isUnsignedNumber(const char* s); // ����С�ַ����Ƿ�Ϊ�޷�����
int isIdentifier(const char* s);  // �Ƿ�Ϊ�Ϸ��ı�ʶ��/�ؼ���
int isSingleSeparator(char c); // �Ƿ�Ϊ���ַ��ָ���
int isDoubleSeparator(const char* s); // �Ƿ�Ϊ˫�ַ��ָ���
int isComment(const char* s);   // �Ƿ�Ϊע��
int isMacro(const char* s);   // �Ƿ�Ϊ��

int get_idx_single(char c);  // �õ����ַ��ָ��������ͱ��
int get_idx_double(const char* double_sep); // �õ�˫�ַ��ָ��������ͱ��

void init_iden(); // ��ʼ��ʶ���ʶ��DFA��״̬ת�ƾ���
void init_unsigned(); // ��ʼ��ʶ���޷�����DFA��״̬ת�ƾ���
void init_double_sep(); // ��ʼ��ʶ��˫�ַ��ָ���DFA��״̬ת�ƾ���
void init_macro(); // ��ʼ��ʶ����DFA��״̬ת�ƾ���
void init_comment(); // ��ʼ��ʶ��ע�͵�DFA��״̬ת�ƾ���