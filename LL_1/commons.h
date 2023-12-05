#ifndef COMMONS_H
#define COMMONS_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MALLOC_NULL_EXIT(pStr) do {\
        if (pStr == NULL) { \
            fprintf(stderr, "(%d) in '%s': Cannot malloc!\n", __LINE__, __FUNCTION__); \
            exit(-1); \
        } \
    } while(0)

#define MAX_SYMBOLS 5
#define RIGHT_ARROW "¡ú"
#define G_S_START "S"
#define NIL "¦Å"
#endif // !COMMONS_H
