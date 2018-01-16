#include "..\common\syscall.h"

unsigned char stack0[1024];
unsigned char stack1[1024];
unsigned char stack2[1024];
unsigned char stack3[1024];

void threadTest0(void) {
    OutputDebugString("Hello from a thread on 0!", 26);

    ExitThread();
}

void threadTest1(void) {
    OutputDebugString("Hello from a thread on 1!", 26);

    ExitThread();
}

void threadTest2(void) {
    OutputDebugString("Hello from a thread on 2!", 26);

    ExitThread();
}

void threadTest3(void) {
    OutputDebugString("Hello from a thread on 3!", 26);

    ExitThread();
}

int main(void) {
/*
    OutputDebugString("Hello world!", 13);

    int coreNum = GetCurrentCoreNumber();

    switch (coreNum) {
    case 0:
        OutputDebugString("0", 2);
        break;
    case 1:
        OutputDebugString("1", 2);
        break;
    case 2:
        OutputDebugString("2", 2);
        break;
    case 3:
        OutputDebugString("3", 2);
        break;
    default:
        OutputDebugString("Error, incorrect core num", 26);
        break;
    }

    int res = ConnectToPort(0, "sm:");

    switch (res) {
    case 3:
        OutputDebugString("3", 2);
        break;
    default:
        OutputDebugString("Error, incorrect res", 21);
        break;
    }
*/
/*
    ipcTest(3);
*/

    unsigned long handle0 = CreateThread(0, threadTest0, 0, stack0 + 1024, 0, 0);
    unsigned long handle1 = CreateThread(0, threadTest1, 0, stack1 + 1024, 0, 1);
    unsigned long handle2 = CreateThread(0, threadTest2, 0, stack2 + 1024, 0, 2);
    unsigned long handle3 = CreateThread(0, threadTest3, 0, stack3 + 1024, 0, 3);
    StartThread(handle0);
    StartThread(handle1);
    StartThread(handle2);
    StartThread(handle3);

    SleepThread(0);

    OutputDebugString("Hello from main!", 17);

    ExitProcess();

    return 1;
}