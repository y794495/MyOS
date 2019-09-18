#ifndef Utility_H
#define Utility_H

#include "Types.h"

void ReadCPUID(DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX);
void SwitchAndExecute64bitKernel();

#endif