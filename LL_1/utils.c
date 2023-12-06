#include "commons.h"

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
