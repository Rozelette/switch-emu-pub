#include "..\common\syscall.h"

int main(void) {
    int i = 5;
    int j = 3;
    int h = i + j;
    h++;

    ExitProcess();

    return 1;
}