#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"

ShellCommandEntry gs_vstCommandTable[]=
{
	{"help","Show Help", Help},
	{"cls", "Clear Screen", Cls},
	{"totalram", "Show Total RAM Size", ShowTotalRAMSize},
	{"strtod","String To Decimal/Hex Convert",StringToDecimalHexTest},
	{"shutdown","Shutdown And Reboot OS", Shutdown},
	{"settimer","Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)",SetTimer},
	{"wait","Wait ms Using PIT, ex)wait 100(ms)",WaitUsingPIT},
	{"rdtsc","Read Time Stamp Counter", ReadTimeStampCounter},
	{"cpuspeed","Measure Processor Speed", MeasureProcessorSpeed},
	{"date","Show Date And Time", ShowDateAndTime},
	{"createtask","Create Task, ex)createtask 1(type) 10(count)", CreateTestTask}
};

void StartConsoleShell()
{
	char vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
	int iCommandBufferIndex = 0;
	BYTE bKey;
	int iCursorX, iCursorY;

	Printf(CONSOLESHELL_PROMPTMESSAGE);

	while (1)
	{
		bKey = GetCh();
		if (bKey == KEY_BACKSPACE)
		{
			if (iCommandBufferIndex > 0)
			{
				GetCursor(&iCursorX, &iCursorY);
				PrintString(iCursorX - 1, iCursorY, " ");
				SetCursor(iCursorX - 1, iCursorY);
				iCommandBufferIndex--;
			}
		}
		else if (bKey == KEY_ENTER)
		{
			Printf("\n");

			if (iCommandBufferIndex > 0)
			{
				vcCommandBuffer[iCommandBufferIndex] = '\0';
				ExecuteCommand(vcCommandBuffer);
			}
			Printf("%s", CONSOLESHELL_PROMPTMESSAGE);

			MemorySet(vcCommandBuffer, '\0', iCommandBufferIndex);
			
			iCommandBufferIndex = 0;
		}
		else if ((bKey == KEY_LSHIFT) || (bKey == KEY_RSHIFT) || (bKey == KEY_CAPSLOCK) || (bKey == KEY_NUMLOCK) || (bKey == KEY_SCROLLLOCK))
		{
			;
		}
		else
		{
			if (bKey == KEY_TAB)
			{
				bKey = ' ';
			}

			if (iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT)
			{
				vcCommandBuffer[iCommandBufferIndex++] = bKey;
				Printf("%c", bKey);
			}
		}
	}
}

void ExecuteCommand(const char* pcCommandBuffer)
{
	int i, iSpaceIndex;
	int iCommandBufferLength, iCommandLength;
	int iCount;

	iCommandBufferLength = StrLen(pcCommandBuffer);
	for (iSpaceIndex = 0; iSpaceIndex < iCommandBufferLength; iSpaceIndex++)
	{
		if (pcCommandBuffer[iSpaceIndex] == ' ')
		{
			break;
		}
	}

	iCount = sizeof(gs_vstCommandTable) / sizeof(ShellCommandEntry);
	for (i = 0; i < iCount; i++)
	{
		iCommandLength = StrLen(gs_vstCommandTable[i].pcCommand);
		if ((iCommandLength == iSpaceIndex) && (MemoryCompare(gs_vstCommandTable[i].pcCommand, pcCommandBuffer, iSpaceIndex) == 0))
		{
			gs_vstCommandTable[i].pfFunction(pcCommandBuffer + iSpaceIndex + 1);
			break;
		}
	}

	if (i >= iCount)
	{
		Printf("'%s' is not found.\n", pcCommandBuffer);
	}
}

void InitializeParameter(ParameterList* pstList, const char* pcParameter)
{
	pstList->pcBuffer = pcParameter;
	pstList->iLength = StrLen(pcParameter);
	pstList->iCurrentPosition = 0;
}


//�������� ���е� �Ķ������ ����� ���̸� ��ȯ
int GetNextParameter(ParameterList* pstList, char* pcParameter)
{
	int i;
	int iLength;

	//���̻� �Ķ���Ͱ� ������ 0�� ��ȯ
	if (pstList->iLength <= pstList->iCurrentPosition)
	{
		return 0;
	}

	//������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
	for (i = pstList->iCurrentPosition; i < pstList->iLength; i++)
	{
		if (pstList->pcBuffer[i] == ' ')
		{
			break;
		}
	}
	//�Ķ���͸� �����ϰ� ���̸� ��ȯ
	MemoryCopy(pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i);
	iLength = i - pstList->iCurrentPosition;
	pcParameter[iLength] = '\0';

	//�Ķ������ ��ġ ������Ʈ
	pstList->iCurrentPosition += iLength + 1;
	return iLength;
}

//Ŀ�ǵ� ó�� �ڵ�

void Help(const char* pcParameterBuffer)
{
	int i;
	int iCount;
	int iCursorX, iCursorY;
	int iLength, iMaxCommandLength = 0;

	Printf("===========================================\n");
	Printf("                 Help List                 \n");
	Printf("===========================================\n");

	iCount = sizeof(gs_vstCommandTable) / sizeof(ShellCommandEntry);

	for (i = 0; i < iCount; i++)
	{
		iLength = StrLen(gs_vstCommandTable[i].pcCommand);
		if (iLength > iMaxCommandLength)
		{
			iMaxCommandLength = iLength;
		}
	}

	for (i = 0; i < iCount; i++)
	{
		Printf("%s", gs_vstCommandTable[i].pcCommand);
		GetCursor(&iCursorX, &iCursorY);
		SetCursor(iMaxCommandLength, iCursorY);
		Printf(" - %s\n", gs_vstCommandTable[i].pcHelp);
	}
}

void Cls(const char* pcParameterBuffer)
{
	ClearScreen();
	SetCursor(0, 1);
}

void ShowTotalRAMSize(const char* pcParameterBuffer)
{
	Printf("Total RAM size = %d MB\n", GetTotalRAMMBSize());
}

void StringToDecimalHexTest(const char* pcParameterBuffer)
{
	char vcParameter[100];
	int iLength;
	ParameterList stList;
	int iCount = 0;
	long lValue;
	
	InitializeParameter(&stList, pcParameterBuffer);

	while (1)
	{
		iLength = GetNextParameter(&stList, vcParameter);
		if (iLength == 0)
		{
			break;
		}

		Printf("Param %d = '%s', Length = %d, ", iCount + 1, vcParameter, iLength);

		if (MemoryCompare(vcParameter, "0x", 2) == 0)
		{
			lValue = AToI(vcParameter + 2, 16);
			Printf("HEX Value = %q\n", lValue);
		}
		else
		{
			lValue = AToI(vcParameter, 10);
			Printf("Decimal Value=%d\n", lValue);
		}

		iCount++;
	}
}

void Shutdown(const char* pcParameterBuffer)
{
	Printf("System Shutdown Start...\n");

	Printf("Press Any Key To Reboot PC...");
	GetCh();
	Reboot();
}

//PIT ��Ʈ�ѷ��� ī���� 0 ����
void SetTimer(const char* pcParameterBuffer)
{
	char vcParameter[100];
	ParameterList stList;
	long lValue;
	BOOL bPeriodic;

	InitializeParameter(&stList, pcParameterBuffer);

	if (GetNextParameter(&stList, vcParameter) == 0)
	{
		Printf("ex)settimer 10(ms) 1(periodic)\n");
		return;
	}
	lValue = AToI(vcParameter, 10);

	if (GetNextParameter(&stList, vcParameter) == 0)
	{
		Printf("ex)settimer 10(ms) 1(periodic)\n");
		return;
	}
	bPeriodic = AToI(vcParameter, 10);
	InitializePIT(MS_TO_COUNT(lValue), bPeriodic);
	Printf("Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic);
}


//PIT ��Ʈ�ѷ��� ���� ����Ͽ� ms ���� ���
void WaitUsingPIT(const char* pcParameterBuffer)
{
	char vcParameter[100];
	int iLength;
	ParameterList stList;
	long lMillisecond;
	int i;

	//�Ķ���� �ʱ�ȭ
	InitializeParameter(&stList, pcParameterBuffer);
	if (GetNextParameter(&stList, vcParameter) == 0)
	{
		Printf("ex)wait 100(ms)\n");
		return;
	}

	lMillisecond = AToI(pcParameterBuffer, 10);
	Printf("%d ms Sleep Start...\n", lMillisecond);

	//���ͷ�Ʈ�� ��Ȱ��ȭ�ϰ� PIT ��Ʈ�ѷ��� ���� ���� �ð��� ����
	DisableInterrupt();
	for (i = 0; i < lMillisecond / 30; i++)
	{
		WaitUsingDirectPIT(MS_TO_COUNT(30));
	}
	WaitUsingDirectPIT(MS_TO_COUNT(lMillisecond % 30));
	EnableInterrupt();
	Printf("%d ms Sleep Complete\n", lMillisecond);

	//Ÿ�̸� ����
	InitializePIT(MS_TO_COUNT(1), TRUE);
}

//Ÿ�� ������ ī���͸� ����
void ReadTimeStampCounter(const char* pcParameterBuffer)
{
	QWORD qwTSC;

	qwTSC = ReadTSC();
	Printf("Time Stamp Counter = %q\n", qwTSC);
}

void MeasureProcessorSpeed(const char* pcParameterBuffer)
{
	int i;
	QWORD qwLastTSC, qwTotalTSC = 0;

	Printf("Now Measuring.");

	//10�� ���� ��ȭ�� Ÿ�� ������ ī���͸� �̿��Ͽ� ���μ����� �ӵ��� ���������� ����
	DisableInterrupt();

	for (i = 0; i < 200; i++)
	{
		qwLastTSC = ReadTSC();
		WaitUsingDirectPIT(MS_TO_COUNT(50));
		qwTotalTSC += ReadTSC() - qwLastTSC;
		Printf(".");
	}
	//Ÿ�̸� ����
	InitializePIT(MS_TO_COUNT(1), TRUE);
	EnableInterrupt();

	Printf("\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000);
}

//RTC ��Ʈ�ѷ��� ����� ���� �� �ð� ������ ǥ��
void ShowDateAndTime(const char* pcParameterBuffer)
{
	BYTE bSecond, bMinute, bHour;
	BYTE bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;

	//RTC ��Ʈ�ѷ����� �ð� �� ���ڸ� ����
	ReadRTCTime(&bHour, &bMinute, &bSecond);
	ReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

	Printf("Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, ConvertDayOfWeekToString(bDayOfWeek));
	Printf("Time: %d:%d:%d\n", bHour, bMinute, bSecond);
}


void TestTask1()
{
	BYTE bData;
	int i = 0, iX = 0, iY = 0, iMargin;
	VideoStruct* pstScreen = (VideoStruct*)CONSOLE_VIDEOMEMORYADDRESS;
	TCB* pstRunningTask;

	//�ڽ��� ID�� �� ȭ�� ���������� ���
	pstRunningTask = GetRunningTask();
	iMargin = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) % 10;

	//ȭ�� �� �����̸� ���鼭 ���� ���
	while (1)
	{
		switch (i)
		{
		case 0:
			iX++;
			if (iX >= (CONSOLE_WIDTH - iMargin))
			{
				i = 1;
			}
			break;
		case 1:
			iY++;
			if (iY >= (CONSOLE_HEIGHT - iMargin))
			{
				i = 2;
			}
			break;

		case 2:
			iX--;
			if (iX < iMargin)
			{
				i = 3;
			}
			break;

		case 3:
			iY--;
			if (iY < iMargin)
			{
				i = 0;
			}
			break;
		}
		//���� �� ���� ����
		pstScreen[iY*CONSOLE_WIDTH + iX].Charactor = bData;
		pstScreen[iY*CONSOLE_WIDTH + iX].Attribute = bData & 0x0F;
		bData++;

		//�ٸ� �½�ũ�� ��ȯ
		Schedule();
	}
}

//�ڽ��� ID�� �����Ͽ� Ư�� ��ġ�� ȸ���ϴ� �ٶ����� ���
void TestTask2()
{
	int i = 0;
	int iOffset;

	VideoStruct* pstScreen = (VideoStruct*)CONSOLE_VIDEOMEMORYADDRESS;
	TCB* pstRunningTask;
	char vcData[4] = { '-','\\','|','/' };

	//�ڽ��� ID�� �� ȭ���� Offset���� ���
	pstRunningTask = GetRunningTask();
	iOffset = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) * 2;
	iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));

	while (1)
	{
		//ȸ���ϴ� �ٶ����� ǥ��
		pstScreen[iOffset].Charactor = vcData[i % 4];
		//���� ����
		pstScreen[iOffset].Attribute = (iOffset % 15) + 1;
		i++;

		//�ٸ� �½�ũ�� ��ȯ
		Schedule();
	}
}

//�½�ũ�� �����ؼ� ��Ƽ�½�ŷ ����
void CreateTestTask(const char* pcParameterBuffer)
{
	ParameterList stList;
	char vcType[30];
	char vcCount[30];
	int i = 0;

	InitializeParameter(&stList, pcParameterBuffer);
	GetNextParameter(&stList, vcType);
	GetNextParameter(&stList, vcCount);

	switch (AToI(vcType, 10))
	{
	case 1://Ÿ��1 �½�ũ ����
		for (i = 0; i < AToI(vcCount, 10); i++)
		{
			if (CreateTask(0, (QWORD)TestTask1) == NULL)
			{
				break;
			}
		}
		Printf("Task1 %d Created\n", i);
		break;

	case 2://Ÿ��2 �½�ũ ����
	default:
		for (i = 0; i < AToI(vcCount, 10); i++)
		{
			if (CreateTask(0, (QWORD)TestTask2) == NULL)
			{
				break;
			}
		}

		Printf("Task2 %d Created\n", i);
		break;
	}
}