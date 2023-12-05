#include "commons.h"

char* trimSpaces(char* str) {
    if (str == NULL) {
        return NULL;
    }
    while (*str == ' ') {
        str++;
    }

    // 如果字符串全是空格
    if (*str == '\0') {
        return _strdup(""); // 返回一个空字符串
    }

    char* end = str + strlen(str) - 1;

    while (end > str && *end == ' ') {
        end--;
    }

    size_t len = end - str + 1;

    char* newStr = (char*)malloc((len + 1) * sizeof(char));
    if (newStr == NULL) {
        return NULL; // 内存分配失败
    }

    strncpy(newStr, str, len);
    newStr[len] = '\0'; // 确保以空字符结束
    return newStr;
}
