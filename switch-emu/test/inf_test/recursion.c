#include "..\common\syscall.h"

int main(void) {
    unsigned long long i = 0;

    while (1) i++;

    ExitProcess();

    return 1;
}