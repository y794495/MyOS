#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"

void CommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode)
{
	char vcBuffer[3] = { 0, };
	
	vcBuffer[0] = '0' + iVectorNumber / 10;
	vcBuffer[1] = '0' + iVectorNumber % 10;
	
	PrintString(0, 0, "==========================================");
	PrintString(0, 1, "               Exception Occur            ");
	PrintString(0, 2, "                   Vector:                ");
	PrintString(27, 2, vcBuffer);
	PrintString(0, 3, "==========================================");
	while (1);
}

void CommonInterruptHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iCommonInterruptCount = 0;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;
	vcBuffer[8] = '0' + g_iCommonInterruptCount;
	g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;
	PrintString(70, 0, vcBuffer);

	SendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);//���ͷ�Ʈ ���� ����
}

void TimerHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iTimerInterruptCount = 0;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;
	vcBuffer[8] = '0' + g_iTimerInterruptCount;
	g_iTimerInterruptCount = (g_iTimerInterruptCount + 1) % 10;
	PrintString(70, 0, vcBuffer);

	SendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);//���ͷ�Ʈ ���� ����

	//Ÿ�̸� �߻� Ƚ���� ����
	g_qwTickCount++;

	//�½�ũ�� ����� ���μ����� �ð��� ����
	DecreaseProcessorTime();
	//���μ����� ����� �� �ִ� �ð��� �ٛ��ٸ� �½�ũ ��ȯ ����
	if (IsProcessorTimeExpired() == TRUE)
	{
		ScheduleInInterrupt();
	}
}

void KeyboardHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:  , ]";
	static int g_iKeyboardInterruptCount = 0;
	BYTE bTemp;

	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;
	vcBuffer[8] = '0' + g_iKeyboardInterruptCount;
	g_iKeyboardInterruptCount = (g_iKeyboardInterruptCount + 1) % 10;
	PrintString(0, 0, vcBuffer);

	if (IsOutputBufferFull() == TRUE)
	{
		bTemp = GetKeyboardScanCode();
		ConvertScanCodeAndPutQueue(bTemp);
	}

	SendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}
