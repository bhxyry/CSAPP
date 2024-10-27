#include <iostream>
using namespace std;
int isAsciiDigit(int x)
{
    int e = ~((1 << 4) - 1);
    int a = x & 0xf0;
    int b = x & 0xf;
    int c = !(a ^ 0x30);
    int d = !!((b + ((~0x9) + 1)) & (1 << 31));
    return c & d;
}
int main()
{
    isAsciiDigit(0x30);
}