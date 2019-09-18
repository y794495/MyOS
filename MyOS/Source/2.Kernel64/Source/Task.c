#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"

static Scheduler gs_stScheduler;
static TCBPoolManager gs_stTCBPoolManager;

void SetUpTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize)
{
	MemorySet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));

	pstTCB->stContext.vqRegister[TASK_RSPOFFSET] = (QWORD)pvStackAddress + qwStackSize;
	pstTCB->stContext.vqRegister[TASK_RBPOFFSET] = (QWORD)pvStackAddress + qwStackSize;

	pstTCB->stContext.vqRegister[TASK_CSOFFSET] = GDT_KERNELCODESEGMENT;
	pstTCB->stContext.vqRegister[TASK_DSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_ESOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_GSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_SSOFFSET] = GDT_KERNELDATASEGMENT;

	pstTCB->stContext.vqRegister[TASK_RIPOFFSET] = qwEntryPointAddress;

	pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET] |= 0x0200;//IF �÷��� Ȱ��ȭ

	pstTCB->pvStackAddress = pvStackAddress;
	pstTCB->qwStackSize = qwStackSize;
	pstTCB->qwFlags = qwFlags;
}

void InitializeTCBPool()
{
	int i;

	MemorySet(&(gs_stTCBPoolManager), 0, sizeof(gs_stTCBPoolManager));

	//�½�ũ Ǯ�� ��巹���� �����ϰ� �ʱ�ȭ
	gs_stTCBPoolManager.pstStartAddress = (TCB*)TASK_TCBPOOLADDRESS;
	MemorySet(TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT);

	//TCB�� ID �Ҵ�
	for (i = 0; i < TASK_MAXCOUNT; i++)
	{
		gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;
	}

	//TCB�� �ִ� ������ �Ҵ�� Ƚ���� �ʱ�ȭ
	gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
	gs_stTCBPoolManager.iAllocatedCount = 1;
}

//TCB�� �Ҵ����
TCB* AllocateTCB()
{
	TCB* pstEmptyTCB;
	int i;

	if (gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount)
	{
		return NULL;
	}

	for (i = 0; i < gs_stTCBPoolManager.iMaxCount; i++)
	{
		//ID�� ���� 32��Ʈ�� 0�̸� �Ҵ���� ���� TCB
		if ((gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID >> 32) == 0)
		{
			pstEmptyTCB = &(gs_stTCBPoolManager.pstStartAddress[i]);
			break;
		}
	}

	//���� 32��Ʈ�� 0�� �ƴ� ������ �����ؼ� �Ҵ�� TCB�� ����
	pstEmptyTCB->stLink.qwID = ((QWORD)gs_stTCBPoolManager.iAllocatedCount << 32) | i;
	gs_stTCBPoolManager.iUseCount++;
	gs_stTCBPoolManager.iAllocatedCount++;
	if (gs_stTCBPoolManager.iAllocatedCount == 0)
	{
		gs_stTCBPoolManager.iAllocatedCount = 1;
	}

	return pstEmptyTCB;
}

//TCB�� ������
void FreeTCB(QWORD qwID)
{
	int i;

	//�½�ũ ID�� ���� 32��Ʈ�� �ε��� ������ ��
	i = qwID & 0xFFFFFFFF;

	//TCB�� �ʱ�ȭ�ϰ� ID ����
	MemorySet(&(gs_stTCBPoolManager.pstStartAddress[i].stContext), 0, sizeof(CONTEXT));
	gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;

	gs_stTCBPoolManager.iUseCount--;
}

//�½�ũ�� ����
//�½�ũ ID�� ���� ���� Ǯ���� ���� �ڵ� �Ҵ�
TCB* CreateTask(QWORD qwFlags, QWORD qwEntryPointAddress)
{
	TCB* pstTask;
	void* pvStackAddress;

	pstTask = AllocateTCB();
	if (pstTask == NULL)
	{
		return NULL;
	}

	//�½�ũ ID�� ���� ��巹�� ���, ���� 32��Ʈ�� ���� Ǯ�� ������ ���� ����
	pvStackAddress = (void*)(TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * (pstTask->stLink.qwID & 0xFFFFFFFF)));

	SetUpTask(pstTask, qwFlags, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
	AddTaskToReadyList(pstTask);

	return pstTask;
}


//�����ٷ� ����
//TCBǮ�� init �½�ũ�� ���� �ʱ�ȭ
void InitializeScheduler()
{
	//�½�ũ Ǯ �ʱ�ȭ
	InitializeTCBPool();

	//�غ� ����Ʈ �ʱ�ȭ
	InitializeList(&(gs_stScheduler.stReadyList));

	//TCB�� �Ҵ�޾� ���� ���� �½�ũ�� �����Ͽ�, ������ ������ �½�ũ�� ������ TCB�� �غ�
	gs_stScheduler.pstRunningTask = AllocateTCB();
}

//���� ���� ���� �½�ũ�� ����
void SetRunningTask(TCB* pstTask)
{
	gs_stScheduler.pstRunningTask = pstTask;
}

//���� ���� ���� �½�ũ�� ��ȯ
TCB* GetRunningTask()
{
	return gs_stScheduler.pstRunningTask;
}

//�½�ũ ����Ʈ���� �������� ������ �½�ũ�� ����
TCB* GetNextTaskToRun()
{
	if (GetListCount(&(gs_stScheduler.stReadyList)) == 0)
	{
		return NULL;
	}

	return (TCB*)RemoveListFromHeader(&(gs_stScheduler.stReadyList));
}

void AddTaskToReadyList(TCB* pstTask)
{
	AddListToTail(&(gs_stScheduler.stReadyList), pstTask);
}

//�ٸ� �½�ũ�� ã�Ƽ� ��ȯ
//���ͷ�Ʈ�� ���ܰ� �߻����� �� ȣ���ϸ� �ȵ�
void Schedule()
{
	TCB* pstRunningTask;
	TCB* pstNextTask;
	BOOL bPreviousFlag;

	//��ȯ�� �½�ũ�� �־�� ��
	if (GetListCount(&(gs_stScheduler.stReadyList)) == 0)
	{
		return;
	}

	//��ȯ�ϴ� ���� ���ͷ�Ʈ�� �߻��Ͽ� �½�ũ ��ȯ�� �� �Ͼ�� ����ϹǷ�
	//��ȯ�ϴ� ���� ���ͷ�Ʈ�� �߻����� ���ϵ��� ����
	bPreviousFlag = SetInterruptFlag(FALSE);
	//������ ���� �½�ũ�� ����
	pstNextTask = GetNextTaskToRun();

	if (pstNextTask == NULL)
	{
		SetInterruptFlag(bPreviousFlag);
		return;
	}

	pstRunningTask = gs_stScheduler.pstRunningTask;
	AddTaskToReadyList(pstRunningTask);

	//���μ��� ��� �ð��� ������Ʈ
	gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

	//���� �½�ũ�� ���� ���� ���� �½�ũ�� ������ �� ���ؽ�Ʈ ��ȯ
	gs_stScheduler.pstRunningTask = pstNextTask;
	SwitchContext(&(pstRunningTask->stContext), &(pstNextTask->stContext));

	SetInterruptFlag(bPreviousFlag);
}

//���ͷ�Ʈ�� �߻����� ��, �ٸ� �½�ũ�� ã�� ��ȯ
//�ݵ�� ���ͷ�Ʈ�� ���ܰ� �߻����� �� ȣ���ؾ� ��
BOOL ScheduleInInterrupt()
{
	TCB* pstRunningTask;
	TCB* pstNextTask;

	char* pcContextAddress;

	//��ȯ�� �½�ũ�� ������ ����
	pstNextTask = GetNextTaskToRun();
	if (pstNextTask == NULL)
	{
		return FALSE;
	}

	//�½�ũ ��ȯ ó��
	//���ͷ�Ʈ �ڵ鷯���� ������ ���ؽ�Ʈ�� �ٸ� ���ؽ�Ʈ�� ����� ������� ó��
	pcContextAddress = (char*)IST_STARTADDRESS + IST_SIZE - sizeof(CONTEXT);

	//���� �½�ũ�� �� IST�� �ִ� ���ؽ�Ʈ�� �����ϰ�, ���� �½�ũ�� �غ� ����Ʈ�� �ű�
	pstRunningTask = gs_stScheduler.pstRunningTask;
	MemoryCopy(&(pstRunningTask->stContext), pcContextAddress, sizeof(CONTEXT));
	AddTaskToReadyList(pstRunningTask);

	//��ȯ�ؼ� ������ �½�ũ�� Running Task�� �����ϰ� ���ؽ�Ʈ�� IST�� �����ؼ�
	//�ڵ����� �½�ũ ��ȯ�� �Ͼ���� ��
	gs_stScheduler.pstRunningTask = pstNextTask;
	MemoryCopy(pcContextAddress, &(pstNextTask->stContext), sizeof(CONTEXT));

	//���μ��� ��� �ð��� ������Ʈ
	gs_stScheduler.pstRunningTask = TASK_PROCESSORTIME;
	return TRUE;
}

//���μ����� ����Ҽ� �ִ� �ð��� �ϳ� ����
void DecreaseProcessorTime()
{
	if (gs_stScheduler.iProcessorTime > 0)
	{
		gs_stScheduler.iProcessorTime--;
	}
}

//���μ����� ����� �� �ִ� �ð��� �� �Ǿ����� ���θ� ��ȯ
BOOL IsProcessorTimeExpired()
{
	if (gs_stScheduler.iProcessorTime <= 0)
	{
		return TRUE;
	}
	return FALSE;
}