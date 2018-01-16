#include "..\common\syscall.h"

void printInt(int n) {
    char buf[11];
    buf[10] = '\0';
    for (int i = 0; i < 10; i++) {
        buf[i] = 'Q';
    }
    for (int i = 9; i >= 0; i--) {
        switch (n % 10) {
        case 9:
            buf[i] = '9';
            break;
        case 8:
            buf[i] = '8';
            break;
        case 7:
            buf[i] = '7';
            break;
        case 6:
            buf[i] = '6';
            break;
        case 5:
            buf[i] = '5';
            break;
        case 4:
            buf[i] = '4';
            break;
        case 3:
            buf[i] = '3';
            break;
        case 2:
            buf[i] = '2';
            break;
        case 1:
            buf[i] = '1';
            break;
        case 0:
            buf[i] = '0';
            break;
        default:
            buf[i] = 'X';
            break;
        }
        n /= 10;
    }
    OutputDebugString(buf, 11);
}

int fib(int n) {
    if (n <= 1) return 1;
    return fib(n - 1) + fib(n - 2);
}

int main(void) {
    printInt(fib(0));
    printInt(fib(1));
    printInt(fib(2));
    printInt(fib(3));
    printInt(fib(4));
    printInt(fib(5));
    printInt(fib(6));
    printInt(fib(7));
    printInt(fib(8));
    printInt(fib(9));
    printInt(fib(10));
    printInt(fib(50));

    ExitProcess();

    return 1;
}