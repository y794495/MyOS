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

	//커서 위치 계산
	iLinearValue = iY * CONSOLE_WIDTH + iX;

	//상위 커서 위치 레지스터를 선택
	OutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
	//CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 상위 바이트를 출력
	OutPortByte(VGA_PORT_DATA, iLinearValue >> 8);

	//CRTC 컨트롤 어드레스 레지스터(포트0x3D4)에 0x0F를 전송하여 하위 커서 위치 레지스터를 선택
	OutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
	//CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 하위 바이트를 출력
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

	//문자열을 출력할 위치를 저장
	iPrintOffset = gs_ConsoleManager.iCurrentPrintOffset;

	iLength = StrLen(pcBuffer);
	for (i = 0; i < iLength; i++)
	{
		if (pcBuffer[i] == '\n')//줄바꿈처리
		{
			iPrintOffset += (CONSOLE_WIDTH - (iPrintOffset % CONSOLE_WIDTH));
		}
		else if (pcBuffer[i] == '\t')//탭 출력
		{
			iPrintOffset += (8 - (iPrintOffset % 8));
		}
		else//일반 문자열 출력
		{
			pScreen[iPrintOffset].Charactor = pcBuffer[i];
			pScreen[iPrintOffset].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
			iPrintOffset++;
		}

		//출력할 위치가 화면의 최댓값을 벗어났으면 스크롤 처리
		if (iPrintOffset >= (CONSOLE_HEIGHT * CONSOLE_WIDTH))
		{
			MemoryCopy(CONSOLE_VIDEOMEMORYADDRESS, CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof(VideoStruct), (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH * sizeof(VideoStruct));
			for (j = (CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH); j < (CONSOLE_HEIGHT * CONSOLE_WIDTH); j++)
			{
				//공백 출력
				pScreen[j].Charactor = ' ';
				pScreen[j].Attribute = CONSOLE_DEFAULT_TEXTCOLOR;
			}
			iPrintOffset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;//출력 위치를 가장 아래쪽 라인의 처음으로 설정
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