#ifndef PageStruct_H
#define PageStruct_H
#include "Types.h"

//���� 32��Ʈ �� �Ӽ� �ʵ�
#define PAGE_FLAGS_P	0x00000001	//Present (��Ʈ�� ��ȿ)
#define PAGE_FLAGS_RW	0x00000002	//Read Write (�б� ���� ����)
#define PAGE_FLAGS_US	0x00000004	//User/Supervisor (Ring3)
#define PAGE_FLAGS_PWT	0x00000008	//Page level - Write - Through
#define PAGE_FLAGS_PCD	0x00000010	//Page level - Cache - Disable
#define PAGE_FLAGS_A	0x00000020	//Access (���� ��)
#define PAGE_FLAGS_D	0x00000040	//Dirty (������ ���Ⱑ �Ǿ��°�)
#define PAGE_FLAGS_PS	0x00000080	/*Page Size
									(PS�� 1�϶� : CR4 ���������� PAE�� 0�̸� Page size�� 4MB, 1�̸� Page size�� 2MB)
									(PS�� 0�϶� : Page size�� 4KB)
									*/
#define PAGE_FLAGS_G	0x00000100	//Global(CR4 ���������� PGE ��Ʈ�� 0�̸�
									//CR3 �������Ͱ��� �ٲ� �ش� �������� ������ ���̺� ĳ���� TLB���� ��ü���� ����)
#define PAGE_FLAGS_PAT	0x00001000	//Page Attribute Table Index(PAT ���ÿ� ���Ǵ� �ֻ��� ��Ʈ)
									//���μ����� PAT�� �����ϸ� PAT, PCD, PWT 3��Ʈ�� ����ؼ� PAT�� ����
									//���μ����� PAT�� �������� ������ 0���� �����
#define PAGE_FLAGS_EXB	0x80000000	//Execute-Disable (���� �������� ������ �������� ����)
									//1�̸� �ش� �������� ������ �������� ����, �ش翵���� �ڵ带 �����ϸ� ������ ��Ʈ ����
									//0�̸� ������� ����
									//IA32_EFER(MSR) ���������� NXE ��Ʈ�� 1�� ������ ��� ��ȿ��

#define PAGE_FLAGS_DEFAULT	(PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLESIZE		0x1000
#define PAGE_MAXENTRYCOUNT	512
#define PAGE_DEFAULTSIZE	0x200000

#pragma pack(push, 1)
typedef struct PageStruct
{
	//PML4T, PDPTE�� ���
	//1��Ʈ P, RW, US, PWT, PCD, A, 3 ��Ʈ Reserved, 3��Ʈ Avail, 20��Ʈ Base Address
	//PDE�� ���
	//1��Ʈ P, RW, US, PWT, PCD, A, D, 1, G, 3��Ʈ Avail, 1��Ʈ PAT, 8��Ʈ Avail, 11��Ʈ Base Address
	DWORD dwAttributeAndLowerBaseAddress;
	//8��Ʈ Upper Base Address, 12 ��Ʈ Reserved, 11 ��Ʈ Avail, 1��Ʈ EXB
	DWORD dwUpperBaseAddressAndEXB;
}PML4TEntry, PDPTEntry, PDEntry, PTEntry;
#pragma pack(pop)

void InitializePageTables();
void SetPageEntryData(PTEntry* pEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);
#endif