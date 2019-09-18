#include "Descriptor.h"
#include "Utility.h"
#include "ISR.h"

void InitializeGDTTableAndTSS()
{
	GDTR* pstGDTR;
	GDTEntry8* pstEntry;
	TSSSegment* pstTSS;
	int i;
	
	pstGDTR = (GDTR*)GDTR_START_ADDRESS;
	pstEntry = (GDTEntry8*)(GDTR_START_ADDRESS + sizeof(GDTR));
	pstGDTR->wLimit = GDT_TABLESIZE - 1;
	pstGDTR->qwBaseAddress = (QWORD)pstEntry;
	pstTSS = (TSSSegment*)((QWORD)pstEntry + GDT_TABLESIZE);
	SetGDTEntry8(&(pstEntry[0]), 0, 0, 0, 0, 0);
	SetGDTEntry8(&(pstEntry[1]), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE);
	SetGDTEntry8(&(pstEntry[2]), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA, GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA);
	SetGDTEntry16((GDTEntry16*)&(pstEntry[3]), (QWORD)pstTSS, sizeof(TSSSegment) - 1, GDT_FLAGS_UPPER_TSS, GDT_FLAGS_LOWER_TSS, GDT_TYPE_TSS);

	InitializeTSSSegment(pstTSS);
}

BYTE WhatIsDPL()
{
	GDTEntry8* pstEntry;

	pstEntry = (GDTEntry8*)(GDTR_START_ADDRESS + sizeof(GDTR));
	return (pstEntry[3]).bTypeAndLowerFlag;
}

void SetGDTEntry8(GDTEntry8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType)
{
	pstEntry->wLowerLimit = dwLimit & 0xFFFF;
	pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;
	pstEntry->bUpperBaseAddress1 = (dwBaseAddress >> 16) & 0xFF;
	pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
	pstEntry->bUpperLimitAndUpperFlag = ((dwLimit >> 16) & 0xFF) | bUpperFlags;
	pstEntry->bUpperBaseAddress2 = (dwBaseAddress >> 24) & 0xFF;
}

void SetGDTEntry16(GDTEntry16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType)
{
	pstEntry->wLowerLimit = dwLimit & 0xFFFF;
	pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;
	pstEntry->bMiddleBaseAddress1 = (qwBaseAddress >> 16) & 0xFF;
	pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
	pstEntry->bUpperLimitAndUpperFlag = ((dwLimit >> 16) & 0xFF) | bUpperFlags;
	pstEntry->bMiddleBaseAddress2 = (qwBaseAddress >> 24) & 0xFF;
	pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;
	pstEntry->dwReserved = 0;
}

void InitializeTSSSegment(TSSSegment* pstTSS)
{
	MemorySet(pstTSS, 0, sizeof(TSSSegment));
	pstTSS->qwIST[0] = IST_STARTADDRESS + IST_SIZE;
	pstTSS->wIOMapBaseAddress = 0xFFFF;//마지막 바이트는 0xFF 필수
}

void InitializeIDTTables()
{
	IDTR* pstIDTR;
	IDTEntry* pstEntry;
	int i;

	pstIDTR = (IDTR*)IDTR_START_ADDRESS;
	pstEntry = (IDTEntry*)(IDTR_START_ADDRESS + sizeof(IDTR));
	pstIDTR->qwBaseAddress = (QWORD)pstEntry;
	pstIDTR->wLimit = IDT_TABLE_SIZE - 1;
	

	SetIDTEntry(&(pstEntry[0]), ISRDivideError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL,IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[1]), ISRDebug, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[2]), ISRNMI, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[3]), ISRBreakPoint, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[4]), ISROverflow, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[5]), ISRBoundRangeExceeded, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[6]), ISRInvalidOpcode, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[7]), ISRDeviceNotAvailable, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[8]), ISRDoubleFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[9]), ISRCoprocessorSegmentOverrun, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[10]), ISRInvalidTSS, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[11]), ISRSegmentNotPresent, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[12]), ISRStackSegmentFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[13]), ISRGeneralProtection, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[14]), ISRPageFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[15]), ISR15, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[16]), ISRFPUError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[17]), ISRAlignmentCheck, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[18]), ISRMachineCheck, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[19]), ISRSIMDError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[20]), ISRETCException, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);

	for (i = 21; i < 32; i++)
	{
		SetIDTEntry(&(pstEntry[i]), ISRETCException, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	}

	SetIDTEntry(&(pstEntry[32]), ISRTimer, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[33]), ISRKeyboard, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[34]), ISRSlavePIC, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[35]), ISRSerial2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[36]), ISRSerial1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[37]), ISRParallel2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[38]), ISRFloppy, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[39]), ISRParallel1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[40]), ISRRTC, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[41]), ISRReserved, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[42]), ISRNotUsed1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[43]), ISRNotUsed2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[44]), ISRMouse, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[45]), ISRCoprocessor, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[46]), ISRHDD1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	SetIDTEntry(&(pstEntry[47]), ISRHDD2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);

	for (i = 48; i < IDT_MAX_ENTRY_COUNT; i++)
	{
		SetIDTEntry(&(pstEntry[i]), ISRETCInterrupt, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
	}
}

void SetIDTEntry(IDTEntry* pstEntry, void* pvHandler, WORD wSelector, BYTE bIST, BYTE bFlags, BYTE bType)
{
	pstEntry->wLowerBaseAddress = (QWORD)pvHandler & 0xFFFF;
	pstEntry->wSegmentSelector = wSelector;
	pstEntry->bIST = bIST & 0x3;
	pstEntry->bTypeAndFlags = bType | bFlags;
	pstEntry->wMiddleBaseAddress = ((QWORD)pvHandler >> 16) & 0xFFFF;
	pstEntry->dwUpperBaseAddress = (QWORD)pvHandler >> 32;
	pstEntry->dwReserved = 0;
}