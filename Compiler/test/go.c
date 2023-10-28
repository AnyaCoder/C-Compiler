#include <stdio.h>

int go_main() {
    int a = 10;
    int b = 20;
    int c;
    /**/
    if (a < b) { // 使用了单字符分界符和保留字
        c = a + b; // 使用了单字符分界符和标识符
    }
    else {
        c = a - b;
    }

    c += a; // 使用了双字符分界符
    c -= b;
    c *= a;
    c /= b;

    int d = c << 2; // 使用了双字符分界符
    int e = d >> 1;

    while (a < b && b > a || a != b) { // 使用了双字符分界符和保留字
        a++;
        b--;
    }

    for (int i = 0; i < 10; i++) { // 使用了单字符分界符和保留字
        printf("%d\n", i);
    }

    do { // 使用了保留字
        a = a + 1;
    } while (a < b); // 使用了单字符分界符和保留字

    return 0; // 使用了单字符分界符和无符号整数
}
