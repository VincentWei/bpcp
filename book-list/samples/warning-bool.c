#include <stdbool.h>
#include <stdio.h>

bool foo(int n, int m)
{
    /* 该逻辑运算符用于非布尔型常数（-Wlogical-op）*/
    return (n & 1 & m && 5);
}

int main(void)
{
    int n = 3, m = 6;

    foo(n, m);

    /* 该逻辑表达式 && 运算符的左右表达式一样（-Wlogical-op）*/
    if (n & 0x01 && n & 0x01) {
        return 1;
    }

    float x = 0.001f;
    double y = 0.001;
    /* 对比两种不同的精度的浮点数可能导致结果不符合预期（-Wfloat-equal） */
    if (x == y) {
        return 1;
    }

    return 0;
}
