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

//�½�ũ Ǯ�� ��巹��
#define TASK_TCBPOOLADDRESS		0x800000
#define TASK_MAXCOUNT			1024

//���� Ǯ�� ������ ũ��
#define TASK_STACKPOOLADDRESS	(TASK_TCBPOOLADDRESS + sizeof(TCB) * TASK_MAXCOUNT)
#define TASK_STACKSIZE			8192

//��ȿ���� ���� �½�ũ ID
#define TASK_INVALIDID			0xFFFFFFFFFFFFFFFF

//�½�ũ�� �ִ�� �� �� �ִ� ���μ��� �ð�
#define TASK_PROCESSORTIME		5

#pragma pack(push,1)

typedef struct ContextStruct
{
	QWORD vqRegister[TASK_REGISTERCOUNT];
}CONTEXT;

typedef struct TaskControlBlockStruct
{
	//���� �������� ��ġ�� ID
	ListLink stLink;

	//�÷���
	QWORD qwFlags;

	//���ؽ�Ʈ
	CONTEXT stContext;

	//������ ��巹���� ũ��
	void* pvStackAddress;
	QWORD qwStackSize;
}TCB;

//TCB Ǯ�� ���¸� �����ϴ� �ڷᱸ��
typedef struct TCBPoolManagerStruct
{
	//�½�ũ Ǯ�� ���� ����
	TCB* pstStartAddress;
	int iMaxCount;
	int iUseCount;

	//TCB�� �Ҵ�� Ƚ��
	int iAllocatedCount;
}TCBPoolManager;

//�����ٷ��� ���¸� �����ϴ� �ڷᱸ��
typedef struct SchedulerStruct
{
	//���� �������� �½�ũ
	TCB* pstRunningTask;

	//���� �������� �½�ũ�� ����� �� �ִ� ���μ��� �ð�
	int iProcessorTime;

	//������ �½�ũ�� �غ� ���� ����Ʈ
	List stReadyList;
}Scheduler;

#pragma pack(pop)

void InitializeTCBPool();
TCB* AllocateTCB();
void FreeTCB(QWORD qwID);
TCB* CreateTask(QWORD qwFlags, QWORD qwEntryPointAddress);
void SetUpTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize);

//�����ٷ�
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