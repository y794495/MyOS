#ifndef ConsoleShell_H
#define ConsoleShell_H

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT	300
#define CONSOLESHELL_PROMPTMESSAGE			"MyOS>"

typedef void(*CommandFunction)(const char* pcParameter);

#pragma pack(push, 1)

typedef struct ShellCommandEntry
{
	//커맨드 문자열
	char* pcCommand;
	//커맨드 도움말
	char* pcHelp;
	//커맨드를 수행하는 함수 포인터
	CommandFunction pfFunction;
}ShellCommandEntry;

typedef struct ParameterList
{
	//파라미터 버퍼의 어드레스
	const char* pcBuffer;
	//파라미터 길이
	int iLength;
	//현재 처리할 파라미터가 시작하는 위치
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