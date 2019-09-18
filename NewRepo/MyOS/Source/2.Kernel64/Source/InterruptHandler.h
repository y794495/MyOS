#ifndef InterruptHandler_H
#define InterruptHandler_H

#include "Types.h"

void CommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void CommonInterruptHandler(int iVectorNumber);
void KeyboardHandler(int iVectorNumber);
void TimerHandler(int iVectorNumber);

#endif