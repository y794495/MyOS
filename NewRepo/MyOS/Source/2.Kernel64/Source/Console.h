#ifndef Console_H
#define Console_H

#include "Types.h"

#define CONSOLE_BACKGROUND_BLACK			0x00
#define CONSOLE_BACKGROUND_BLUE				0x10
#define CONSOLE_BACKGROUND_GREEN			0x20
#define CONSOLE_BACKGROUND_CYAN				0x30
#define CONSOLE_BACKGROUND_RED				0x40
#define CONSOLE_BACKGROUND_MAGENTA			0x50
#define CONSOLE_BACKGROUND_BROWN			0x60
#define CONSOLE_BACKGROUND_WHITE			0x70
#define CONSOLE_BACKGROUND_BLINK			0x80
#define CONSOLE_TEXT_DARKBLACK				0x00
#define CONSOLE_TEXT_DARKBLUE				0x01
#define CONSOLE_TEXT_DARKGREEN				0x02
#define CONSOLE_TEXT_DARKCYAN				0x03
#define CONSOLE_TEXT_DARKRED				0x04
#define CONSOLE_TEXT_DARKMAGENTA			0x05
#define CONSOLE_TEXT_DARKBROWN				0x06
#define CONSOLE_TEXT_DARKWHITE				0x07
#define CONSOLE_TEXT_BRIGHTBLACK			0x08
#define CONSOLE_TEXT_BRIGHTBLUE				0x09
#define CONSOLE_TEXT_BRIGHTGREEN			0x0A
#define CONSOLE_TEXT_BRIGHTCYAN				0x0B
#define CONSOLE_TEXT_BRIGHTRED				0x0C
#define CONSOLE_TEXT_BRIGHTMAGENTA			0x0D
#define CONSOLE_TEXT_BRIGHTYELLOW			0x0E
#define CONSOLE_TEXT_BRIGHTWHITE			0x0F

#define CONSOLE_DEFAULT_TEXTCOLOR			(CONSOLE_BACKGROUND_BLACK | CONSOLE_TEXT_BRIGHTWHITE)

#define CONSOLE_WIDTH				80
#define CONSOLE_HEIGHT				25
#define CONSOLE_VIDEOMEMORYADDRESS	0xB8000

#define VGA_PORT_INDEX				0x3D4
#define VGA_PORT_DATA				0x3D5
#define VGA_INDEX_UPPERCURSOR		0x0E
#define VGA_INDEX_LOWERCURSOR		0x0F

#pragma pack(push, 1)
typedef struct ConsoleManager
{
	int iCurrentPrintOffset;
}ConsoleManager;

#pragma pack(pop)

void InitializeConsole(int iX, int iY);
void SetCursor(int iX, int iY);
void GetCursor(int* piX, int* piY);
void Printf(const char* pcFormatString, ...);
int ConsolePrintString(const char* pcBuffer);
void ClearScreen();
BYTE GetCh();
void PrintString(int iX, int iY, const char* pcString);

#endif