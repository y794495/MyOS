#ifndef Utility_H
#define Utility_H

#include <stdarg.h>
#include "Types.h"

void MemorySet(void* pvDestination, BYTE bData, int iSize);
int MemoryCopy(void* pvDestination, const void* pvSource, int iSize);
int MemoryCompare(const void* pvDestination, const void* pvSource, int iSize);
BOOL SetInterruptFlag(BOOL bEnableInterrupt);

int StrLen(const char* pcBuffer);
void CheckTotalRAMSize();
QWORD GetTotalRAMMBSize();
long AToI(const char* pcBuffer, int iRadix);
QWORD HexStringToQword(const char* pcBuffer);
long DecimalStringToLong(const char* pcBuffer);
int IToA(long lValue, char* pcBuffer, int iRadix);
int HexToString(QWORD qwValue, char* pcBuffer);
int DecimalToString(long lValue, char* pcBuffer);
void ReverseString(char* pcBuffer);
int SPrintf(char* pcBuffer, const char* pcFormatString, ...);
int VSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap);
QWORD GetTickCount();

extern volatile QWORD g_qwTickCount;

#endif