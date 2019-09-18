#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"

void print(int x, int y, const char* string);

void Main(void)
{
	int iCursorX, iCursorY;
	InitializeConsole(0, 10);
	Printf("Switch To IA-32e Mode Success\n");
	Printf("IA-32e C Language Kernel start...................[Pass]\n");
	Printf("Initialize Console...............................[Pass]\n");
	
	GetCursor(&iCursorX, &iCursorY);
	Printf("GDT Initialize And Switch For IA-32e Mode........[    ]");
	InitializeGDTTableAndTSS();
	LoadGDTR(GDTR_START_ADDRESS);
	SetCursor(50, iCursorY++);
	Printf("Pass\n");
	
	Printf("TSS Segment Load.................................[    ]");
	LoadTR(GDT_TSSSEGMENT);
	SetCursor(50, iCursorY++);
	Printf("Pass\n");

	Printf("IDT Initialize...................................[    ]");
	InitializeIDTTables();
	LoadIDTR(IDTR_START_ADDRESS);
	SetCursor(50, iCursorY++);
	Printf("Pass\n");

	Printf("Total RAM Size Check.............................[    ]");
	CheckTotalRAMSize();
	SetCursor(50, iCursorY++);
	Printf("Pass], Size = %d MB\n",GetTotalRAMMBSize());

	Printf("TCB Pool And Scheduler Initialize................[Pass]\n");
	iCursorY++;
	InitializeScheduler();
	InitializePIT(MS_TO_COUNT(1), 1);

	Printf("Keyboard Active And Queue Initialize.............[    ]");
	
	if (InitializeKeyboard() == TRUE)
	{
		SetCursor(50, iCursorY++);
		Printf("Pass\n");
		ChangeKeyboardLED(FALSE, FALSE, FALSE);
	}
	else
	{
		SetCursor(50, iCursorY++);
		Printf("Fail\n");
		while (1);
	}
	
	Printf("PIC Controller And Interrupt Initialize..........[    ]");
	InitializePIC();
	MaskPICInterrupt(0);
	EnableInterrupt();
	SetCursor(50, iCursorY++);
	Printf("Pass\n");

	StartConsoleShell();
}

void print(int x, int y, const char* string)
{
	VideoStruct* Screen = (VideoStruct*)0xB8000;
	int i;

	Screen += (y * 80) + x;

	for (i = 0; string[i] != 0; i++)
	{
		Screen[i].Charactor = string[i];
	}
}