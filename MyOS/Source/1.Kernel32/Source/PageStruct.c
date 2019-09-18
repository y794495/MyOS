#include "PageStruct.h"

void InitializePageTables()
{
	PML4TEntry* pPML4TEntry;
	PDPTEntry* pPDPTEntry;
	PDEntry* pPDEntry;
	DWORD dwMappingAddress;
	int i;

	pPML4TEntry = (PML4TEntry*)0x100000;
	SetPageEntryData(&(pPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
	for (i = 1; i < PAGE_MAXENTRYCOUNT; i++)
	{
		SetPageEntryData(&(pPML4TEntry[i]), 0, 0, 0, 0);
	}

	pPDPTEntry = (PDPTEntry*)0x101000;
	for (i = 0; i < 64; i++)
	{
		SetPageEntryData(&(pPDPTEntry[i]), 0x00, 0x102000 + (i * PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
	}

	for (i = 64; i < PAGE_MAXENTRYCOUNT; i++)
	{
		SetPageEntryData(&(pPDPTEntry[i]), 0, 0, 0, 0);
	}

	pPDEntry = (PDEntry*)0x102000;
	dwMappingAddress = 0;
	for (i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
	{
		SetPageEntryData(&(pPDEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}
}

void SetPageEntryData(PTEntry* pEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{
	pEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}