#include "Types.h"
#include "PageStruct.h"
#include "Switch64.h"

void print(int x, int y, const char* string);
BOOL InitializeKernel64Area();
BOOL IsMemoryEnough();
void CopyKernel64ImageTo2Mbyte();

void Main(void)
{
	DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[13] = { 0, };
	print(0, 3, "Protected Mode C Language Kernel start...........[Pass]");
	print(0, 4, "Minimum Memory Size Check........................[    ]");
	if (IsMemoryEnough() == FALSE)
	{
		print(50, 4, "Fail");
		print(0, 5, "Not Enough Memory");
		while (1);
	}
	else
	{
		print(50, 4, "Pass");
	}

	

	print(0, 5, "IA-32e Kernel Area Initialize....................[    ]");
	if (InitializeKernel64Area() == FALSE)
	{
		print(50, 5, "Fail");
		print(0, 6, "Kernel Area Initialization Fail");
		while (1);
	}
	print(50, 5, "Pass");

	print(0, 6, "IA-32e Page Tables Initialize....................[    ]");
	InitializePageTables();
	print(50, 6, "Pass");

	ReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*(DWORD*)vcVendorString = dwEBX;
	*((DWORD*)vcVendorString + 1) = dwEDX;
	*((DWORD*)vcVendorString + 2) = dwECX;
	print(0, 7, "Processor Vendor String..........................[            ]");
	print(50, 7, vcVendorString);

	ReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	print(0, 8, "64bit Mode Support Check.........................[    ]");
	if ( dwEDX & (1 << 29) )
	{
		print(50, 8, "Pass");
	}
	else
	{
		print(50, 8, "Fail");
		print(0, 9, "This Processor does not support 64bit mode");
		while (1);
	}

	print(0, 9, "Copy IA-32e Kernel To 2M address.................[    ]");
	CopyKernel64ImageTo2Mbyte();
	print(50, 9, "Pass");

	print(0, 10, "Switch To IA-32e Mode");
	SwitchAndExecute64bitKernel();
	while (1);
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

BOOL InitializeKernel64Area()
{
	DWORD* pdwCurrentAddress;

	pdwCurrentAddress = (DWORD*)0x100000;

	while ((DWORD)pdwCurrentAddress < 0x600000)
	{
		*pdwCurrentAddress = 0x00;

		if (*pdwCurrentAddress != 0)
		{
			return FALSE;
		}

		pdwCurrentAddress++;
	}
	return TRUE;
}

BOOL IsMemoryEnough()
{
	DWORD* pdwCurrentAddress;

	pdwCurrentAddress = (DWORD*)0x100000;

	while ((DWORD)pdwCurrentAddress < 0x4000000)
	{
		*pdwCurrentAddress = 0x12345678;
		if (*pdwCurrentAddress != 0x12345678)
		{
			return FALSE;
		}

		pdwCurrentAddress += (0x100000 / 4);
	}
	return TRUE;
}



void CopyKernel64ImageTo2Mbyte()
{
	WORD wKernel32SectorCount, wTotalKernelSectorCount;
	DWORD* pdwSourceAddress;
	DWORD* pdwDestinationAddress;
	int i;

	wTotalKernelSectorCount = *((WORD*)0x7c05);
	wKernel32SectorCount = *((WORD*)0x7c07);

	pdwSourceAddress = (DWORD*)(0x10000 + (wKernel32SectorCount * 512));
	pdwDestinationAddress = (DWORD*)0x200000;

	for (i = 0; i < 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4; i++)
	{
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}

