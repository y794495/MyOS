#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

ConsoleManager gs_ConsoleManager = { 0, };

void InitializeConsole(int iX, int iY)
{
	MemorySet(&gs_ConsoleManager, 0, sizeof(gs_ConsoleManager));

	SetCursor(iX, iY);
}

void SetCursor(int iX, int iY)
{
	int iLinearValue;

	//Ŀ�� ��ġ ���
	iLinearValue = iY * CONSOLE_WIDTH + iX;

	//���� Ŀ�� ��ġ �������͸� ����
	OutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
	//CRTC ��Ʈ�� ������ ��������(��Ʈ 0x3D5)�� Ŀ���� ���� ����Ʈ�� ���
	OutPortByte(VGA_PORT_DATA, iLinearValue >> 8);

	//CRTC ��Ʈ�� ��巹�� ��������(��Ʈ0x3D4)�� 0x0F�� �����Ͽ� ���� Ŀ�� ��ġ �������͸� ����
	OutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
	//CRTC ��Ʈ�� ������ ��������(��Ʈ 0x3D5)�� Ŀ���� ���� ����Ʈ�� ���
	OutPortByte(VGA_PORT_DATA, iLinearValue & 0xFF);

	gs_ConsoleManager.iCurrentPrintOffset = iLinearValue;
}

void GetCursor(int* piX, int* piY)
{
	*piX = gs_ConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
	*piY = gs_ConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

void Printf(const char* pcFormatString, ...)
{
	va_list ap;
	char vcBuffer[1024];
	int iNextPrintOffset;

	va_start(ap, pcFormatString);
	VSPrintf(vcBuffer, pcFormatString, ap);
	va_end(ap);

	iNextPrintOffset = ConsolePrintString(vcBuffer);

	SetCursor(iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH);
}

int ConsolePrintString(const char* pcBuffer)
{
	VideoStruct* pScreen = (VideoStruct*)CONSOLE_VIDEOMEMORYADDRESS;
	int i, j;
	int iLength;
	int iPrintOffset;

	//���ڿ��� ����� ��ġ�� ����
	iPrintOffset = gs_ConsoleManager.iCurrentPrintOffset;

	iLength = StrLen(pcBuffer);
	for (i = 0; i < iLength; i++)
	{
		if (pcBuffer[i] == '\n')//�ٹٲ�ó��
		{
			iPrintOffset += (CONSOLE_WIDTH - (iPrintOffset % CONSOLE_WIDTH));
		}
		else if (pcBuffer[i] == '\t')//�� ���
		{
			iPrintOffset += (8 - (iPrintOffset % 8));
		}
		else//�Ϲ� ���ڿ� ���
		{
			pScreen[iPrintOffset].Charactor = pcBuffer[i];
			pScreen[iPrintOffset].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
			iPrintOffset++;
		}

		//����� ��ġ�� ȭ���� �ִ��� ������� ��ũ�� ó��
		if (iPrintOffset >= (CONSOLE_HEIGHT * CONSOLE_WIDTH))
		{
			MemoryCopy(CONSOLE_VIDEOMEMORYADDRESS, CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof(VideoStruct), (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH * sizeof(VideoStruct));
			for (j = (CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH); j < (CONSOLE_HEIGHT * CONSOLE_WIDTH); j++)
			{
				//���� ���
				pScreen[j].Charactor = ' ';
				pScreen[j].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
			}
			iPrintOffset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;//��� ��ġ�� ���� �Ʒ��� ������ ó������ ����
		}
	}
	return iPrintOffset;
}

void ClearScreen()
{
	VideoStruct* pScreen = (VideoStruct*)CONSOLE_VIDEOMEMORYADDRESS;
	int i;

	for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++)
	{
		pScreen[i].Charactor = ' ';
		pScreen[i].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
	}

	SetCursor(0, 0);
}

BYTE GetCh()
{
	KeyData Data;

	while (1)
	{
		while (GetKeyFromKeyQueue(&Data) == FALSE)
		{
			;
		}

		if (Data.bFlags & KEY_FLAGS_DOWN)
		{
			return Data.bASCIICode;
		}
	}
}

void PrintString(int iX, int iY, const char* pcString)
{
	VideoStruct* pScreen = (VideoStruct*)CONSOLE_VIDEOMEMORYADDRESS;
	int i;

	pScreen += (iY * 80) + iX;

	for (i = 0; pcString[i] != 0; i++)
	{
		pScreen[i].Charactor = pcString[i];
		pScreen[i].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
	}
}