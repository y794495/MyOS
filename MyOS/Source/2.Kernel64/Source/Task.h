#ifndef Task_H
#define Task_H

#include "Types.h"
#include "List.h"

#define TASK_REGISTERCOUNT	(5+19)
#define TASK_REGISTERSIZE	8

enum
{
	TASK_GSOFFSET = 0,
	TASK_FSOFFSET,
	TASK_ESOFFSET,
	TASK_DSOFFSET,
	TASK_R150FFSET,
	TASK_R140FFSET,
	TASK_R130FFSET,
	TASK_R120FFSET,
	TASK_R110FFSET,
	TASK_R100FFSET,
	TASK_R90FFSET,
	TASK_R80FFSET,
	TASK_RSIOFFSET,
	TASK_RDIOFFSET,
	TASK_RDXOFFSET,
	TASK_RCXOFFSET,
	TASK_RBXOFFSET,
	TASK_RAXOFFSET,
	TASK_RBPOFFSET,
	TASK_RIPOFFSET,
	TASK_CSOFFSET,
	TASK_RFLAGSOFFSET,
	TASK_RSPOFFSET,
	TASK_SSOFFSET
};

//태스크 풀의 어드레스
#define TASK_TCBPOOLADDRESS		0x800000
#define TASK_MAXCOUNT			1024

//스택 풀과 스택의 크기
#define TASK_STACKPOOLADDRESS	(TASK_TCBPOOLADDRESS + sizeof(TCB) * TASK_MAXCOUNT)
#define TASK_STACKSIZE			8192

//유효하지 않은 태스크 ID
#define TASK_INVALIDID			0xFFFFFFFFFFFFFFFF

//태스크가 최대로 쓸 수 있는 프로세서 시간
#define TASK_PROCESSORTIME		5

#pragma pack(push,1)

typedef struct ContextStruct
{
	QWORD vqRegister[TASK_REGISTERCOUNT];
}CONTEXT;

typedef struct TaskControlBlockStruct
{
	//다음 데이터의 위치와 ID
	ListLink stLink;

	//플래그
	QWORD qwFlags;

	//콘텍스트
	CONTEXT stContext;

	//스택의 어드레스와 크기
	void* pvStackAddress;
	QWORD qwStackSize;
}TCB;

//TCB 풀의 상태를 관리하는 자료구조
typedef struct TCBPoolManagerStruct
{
	//태스크 풀에 대한 정보
	TCB* pstStartAddress;
	int iMaxCount;
	int iUseCount;

	//TCB가 할당된 횟수
	int iAllocatedCount;
}TCBPoolManager;

//스케줄러의 상태를 관리하는 자료구조
typedef struct SchedulerStruct
{
	//현재 수행중인 태스크
	TCB* pstRunningTask;

	//현재 수행중인 태스크가 사용할 수 있는 프로세서 시간
	int iProcessorTime;

	//실행할 태스크가 준비 중인 리스트
	List stReadyList;
}Scheduler;

#pragma pack(pop)

void InitializeTCBPool();
TCB* AllocateTCB();
void FreeTCB(QWORD qwID);
TCB* CreateTask(QWORD qwFlags, QWORD qwEntryPointAddress);
void SetUpTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize);

//스케줄러
void InitializeScheduler();
void SetRunningTask(TCB* pstTask);
TCB* GetRunningTask();
TCB* GetNextTaskToRun();
void AddTaskToReadyList(TCB* pstTask);
void Schedule();
BOOL ScheduleInInterrupt();
void DecreaseProcessorTime();
BOOL IsProcessorTimeExpired();

#endif