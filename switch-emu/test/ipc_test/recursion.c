#include "..\common\syscall.h"

int main(void) {
    int res = ConnectToPort(0, "sm:");

    ipcTest(res);

    ExitProcess();

    return 1;
}