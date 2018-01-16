#include "..\common\syscall.h"

int main(void) {
    unsigned int i1 = 0xFFFFFFFF;
    float f1 = i1;

    float f2  = 4.3;
    unsigned int i2 = (unsigned int)f2 + (unsigned int)f2;

    ExitProcess();

    return 1;
}