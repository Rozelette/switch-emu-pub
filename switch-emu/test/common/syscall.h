#pragma once

void ExitProcess(void); // 0x07
unsigned long CreateThread(int unk, void (*entry)(void), unsigned long long arg, unsigned char* stack, unsigned int priority, unsigned int processorID); // 0x08
void StartThread(unsigned long handle); // 0x09
void ExitThread(); // 0x0A
void SleepThread(unsigned long long nano); // 0x0B
int GetCurrentCoreNumber(void); // 0x10
int ConnectToPort(int unk, char* name); // 0x1F
int SendSyncRequest(int handle); // 0x21
void OutputDebugString(char const* str, unsigned long long size); // 0x27

unsigned long long getTLS();

// temp
int ipcTest(int handle) {
    unsigned int* buffer = (unsigned int*)getTLS();
    buffer[0] = 4;
    buffer[1] = 0x8000000A;
    buffer[2] = 1;
    SendSyncRequest(handle);
}