#pragma once

void ExitProcess(void); // 0x07
int GetCurrentCoreNumber(void); // 0x10
unsigned long int ConnectToPort(int unk, char* name); // 0x1F
void OutputDebugString(char const* str, unsigned long long size); // 0x27