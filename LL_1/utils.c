#include "commons.h"

int total_keys = 0;
char* trimSpaces(char* str) {
    if (str == NULL) {
        return NULL;
    }
    while (*str == ' ') {
        str++;
    }

    // ����ַ���ȫ�ǿո�
    if (*str == '\0') {
        return _strdup(""); // ����һ�����ַ���
    }

    char* end = str + strlen(str) - 1;

    while (end > str && *end == ' ') {
        end--;
    }

    size_t len = end - str + 1;

    char* newStr = (char*)malloc((len + 1) * sizeof(char));
    if (newStr == NULL) {
        return NULL; // �ڴ����ʧ��
    }

    strncpy(newStr, str, len);
    newStr[len] = '\0'; // ȷ���Կ��ַ�����
    return newStr;
}



// �����½ڵ�
TrieNode* newTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    MALLOC_NULL_EXIT(node);
    node->isEndOfWord = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;
    return node;
}

int getIndexOfKey() {
    return total_keys++;
}

int getIndexOfChar(int chr) {

    switch (chr)
    {
    case '��':
        return 52;
    case '\'':
        return 53;
    case ';':
        return 54;
    case '=':
        return 55;
    case '+':
        return 56;
    case '-':
        return 57;
    case '*':
        return 58;
    case '/':
        return 59;
    case '$':
        return 60;
    case '(':
        return 61;
    case ')':
        return 62;
    default:
        if (isalpha(chr)) {
            if (isupper(chr)) {
                return chr - 'A';
            }
            else {
                return chr - 'a' + 26;
            }
        }
        return 63;
    }
}
// ������Ų���ȡ������
int insertAndGetIndex(TrieNode* root, char* key) {
    TrieNode* pCrawl = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int index = getIndexOfChar(key[i]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = newTrieNode();
        pCrawl = pCrawl->children[index];
    }
    if (!pCrawl->isEndOfWord) {
        pCrawl->isEndOfWord = 1;
        pCrawl->index = getIndexOfKey();/* ����Ψһ��� */;
    }
    return pCrawl->index;
}
