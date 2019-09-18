#ifndef Descriptor_H
#define Descriptor_H

#include "Types.h"

//GDT

#define GDT_TYPE_CODE			0x0A
#define GDT_TYPE_DATA			0x02
#define GDT_TYPE_TSS			0x09
#define GDT_FLAGS_LOWER_S		0x10
#define GDT_FLAGS_LOWER_DPL0	0x00
#define GDT_FLAGS_LOWER_DPL1	0x20
#define GDT_FLAGS_LOWER_DPL2	0x40
#define GDT_FLAGS_LOWER_DPL3	0x60
#define GDT_FLAGS_LOWER_P		0x80
#define GDT_FLAGS_UPPER_L		0x20
#define GDT_FLAGS_UPPER_DB		0x40
#define GDT_FLAGS_UPPER_G		0x80

#define GDT_FLAGS_LOWER_KERNELCODE	(GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_KERNELDATA	(GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_TSS			(GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_USERCODE	(GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_USERDATA	(GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P)

#define GDT_FLAGS_UPPER_CODE		(GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_DATA		(GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_TSS			(GDT_FLAGS_UPPER_G)

#define GDT_KERNELCODESEGMENT		0x08
#define GDT_KERNELDATASEGMENT		0x10
#define GDT_TSSSEGMENT				0x18

#define GDTR_START_ADDRESS			0x142000
#define GDT_MAX_ENTRY_8_COUNT		3
#define GDT_MAX_ENTRY_16_COUNT		1

#define GDT_TABLESIZE				((sizeof(GDTEntry8) * GDT_MAX_ENTRY_8_COUNT) + (sizeof(GDTEntry16) * GDT_MAX_ENTRY_16_COUNT))
#define TSS_SEGMENTSIZE				(sizeof(TSSSegment))

//IDT

#define IDT_TYPE_INTERRUPT		0x0E
#define IDT_TYPE_TRAP			0x0F
#define IDT_FLAGS_DPL0			0x00
#define IDT_FLAGS_DPL1			0x20
#define IDT_FLAGS_DPL2			0x40
#define IDT_FLAGS_DPL3			0x60
#define IDT_FLAGS_P				0x80
#define IDT_FLAGS_IST0			0
#define IDT_FLAGS_IST1			1

#define IDT_FLAGS_KERNEL		(IDT_FLAGS_DPL0 | IDT_FLAGS_P)
#define IDT_FLAGS_USER			(IDT_FLAGS_DPL3 | IDT_FLAGS_P)

#define IDT_MAX_ENTRY_COUNT		100
#define IDTR_START_ADDRESS		(GDTR_START_ADDRESS + sizeof(GDTR) + GDT_TABLESIZE + TSS_SEGMENTSIZE)
#define IDT_START_ADDRESS		(IDTR_START_ADDRESS + sizeof(IDTR))

#define IDT_TABLE_SIZE			(IDT_MAX_ENTRY_COUNT * sizeof(IDTEntry))

#define IST_STARTADDRESS		0x700000
#define IST_SIZE				0x100000

#pragma pack(push, 1)

typedef struct GDTRStruct
{
	WORD wLimit;
	QWORD qwBaseAddress;
	WORD wPading;
	DWORD dwPading;
}GDTR, IDTR;

typedef struct GDTEntry8Struct
{
	WORD wLowerLimit;
	WORD wLowerBaseAddress;
	BYTE bUpperBaseAddress1;	//4비트 TYPE, 1비트 S, 2비트 DPL, 1비트 P
	BYTE bTypeAndLowerFlag;		//4비트 Segment Limit, 1비트 AVL, L, D/B, G
	BYTE bUpperLimitAndUpperFlag;
	BYTE bUpperBaseAddress2;
}GDTEntry8;

typedef struct GDTEntry16Struct
{
	WORD wLowerLimit;
	WORD wLowerBaseAddress;
	BYTE bMiddleBaseAddress1;	//4비트 TYPE, 1비트 0, 2비트 DPL, 1비트 P
	BYTE bTypeAndLowerFlag;		//4비트 Segment Limit, 1bit AVL, 0, 0, G
	BYTE bUpperLimitAndUpperFlag;
	BYTE bMiddleBaseAddress2;
	DWORD dwUpperBaseAddress;
	DWORD dwReserved;
}GDTEntry16;

typedef struct TSSDataStruct
{
	DWORD dwReserved1;
	QWORD qwRsp[3];
	QWORD qwReserved2;
	QWORD qwIST[7];
	QWORD qwReserved3;
	WORD wReserved;
	WORD wIOMapBaseAddress;
}TSSSegment;

typedef struct IDTEntryStuct
{
	WORD wLowerBaseAddress;
	WORD wSegmentSelector;
	BYTE bIST;
	BYTE bTypeAndFlags;
	WORD wMiddleBaseAddress;
	DWORD dwUpperBaseAddress;
	DWORD dwReserved;
}IDTEntry;

#pragma pack (pop)

void InitializeGDTTableAndTSS();
void SetGDTEntry8(GDTEntry8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType);
void SetGDTEntry16(GDTEntry16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType);
void InitializeTSSSegment(TSSSegment* pstTSS);
void InitializeIDTTables();
void SetIDTEntry(IDTEntry* psstEntry, void* pvHandler, WORD wSelector, BYTE bIST, BYTE bFlags, BYTE bType);
BYTE WhatIsDPL();
#endif