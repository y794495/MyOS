#ifndef ConsoleShell_H
#define ConsoleShell_H

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT	300
#define CONSOLESHELL_PROMPTMESSAGE			"MyOS>"

typedef void(*CommandFunction)(const char* pcParameter);

#pragma pack(push, 1)

typedef struct ShellCommandEntry
{
	//Ŀ�ǵ� ���ڿ�
	char* pcCommand;
	//Ŀ�ǵ� ����
	char* pcHelp;
	//Ŀ�ǵ带 �����ϴ� �Լ� ������
	CommandFunction pfFunction;
}ShellCommandEntry;

typedef struct ParameterList
{
	//�Ķ���� ������ ��巹��
	const char* pcBuffer;
	//�Ķ���� ����
	int iLength;
	//���� ó���� �Ķ���Ͱ� �����ϴ� ��ġ
	int iCurrentPosition;
}ParameterList;

#pragma pack(pop)

void StartConsoleShell();
void ExecuteCommand(const char* pcCommandBuffer);
void InitializeParameter(ParameterList* pstList, const char* pcParameter);
int GetNextParameter(ParameterList* pstList, char* pcParameter);

void Help(const char* pcParameterBuffer);
void Cls(const char* pcParameterBuffer);
void ShowTotalRAMSize(const char* pcParameterBuffer);
void StringToDecimalHexTest(const char* pcParameterBuffer);
void Shutdown(const char* pcParameterBuffer);
void SetTimer(const char* pcParameterBuffer);
void WaitUsingPIT(const char* pcParameterBuffer);
void ReadTimeStampCounter(const char* pcParameterBuffer);
void MeasureProcessorSpeed(const char* pcParameterBuffer);
void ShowDateAndTime(const char* pcParameterBuffer);
void CreateTestTask(const char* pcParameterBuffer);

#endif