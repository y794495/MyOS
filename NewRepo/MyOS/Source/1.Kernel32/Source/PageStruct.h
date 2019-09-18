#ifndef PageStruct_H
#define PageStruct_H
#include "Types.h"

//하위 32비트 용 속성 필드
#define PAGE_FLAGS_P	0x00000001	//Present (엔트리 유효)
#define PAGE_FLAGS_RW	0x00000002	//Read Write (읽기 쓰기 가능)
#define PAGE_FLAGS_US	0x00000004	//User/Supervisor (Ring3)
#define PAGE_FLAGS_PWT	0x00000008	//Page level - Write - Through
#define PAGE_FLAGS_PCD	0x00000010	//Page level - Cache - Disable
#define PAGE_FLAGS_A	0x00000020	//Access (접근 됨)
#define PAGE_FLAGS_D	0x00000040	//Dirty (페이지 쓰기가 되었는가)
#define PAGE_FLAGS_PS	0x00000080	/*Page Size
									(PS가 1일때 : CR4 레지스터의 PAE가 0이면 Page size는 4MB, 1이면 Page size는 2MB)
									(PS가 0일때 : Page size는 4KB)
									*/
#define PAGE_FLAGS_G	0x00000100	//Global(CR4 레지스터의 PGE 비트가 0이면
									//CR3 레지스터값이 바뀌어도 해당 페이지를 페이지 테이블 캐시인 TLB에서 교체하지 않음)
#define PAGE_FLAGS_PAT	0x00001000	//Page Attribute Table Index(PAT 선택에 사용되는 최상위 비트)
									//프로세서가 PAT를 지원하면 PAT, PCD, PWT 3비트를 사용해서 PAT를 선택
									//프로세서가 PAT를 지원하지 않으면 0으로 예약됨
#define PAGE_FLAGS_EXB	0x80000000	//Execute-Disable (관련 페이지를 데이터 전용으로 설정)
									//1이면 해당 페이지를 데이터 전용으로 설정, 해당영역에 코드를 실행하면 페이지 폴트 예외
									//0이면 제약사항 없음
									//IA32_EFER(MSR) 레지스터의 NXE 비트를 1로 설정할 경우 유효함

#define PAGE_FLAGS_DEFAULT	(PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLESIZE		0x1000
#define PAGE_MAXENTRYCOUNT	512
#define PAGE_DEFAULTSIZE	0x200000

#pragma pack(push, 1)
typedef struct PageStruct
{
	//PML4T, PDPTE의 경우
	//1비트 P, RW, US, PWT, PCD, A, 3 비트 Reserved, 3비트 Avail, 20비트 Base Address
	//PDE의 경우
	//1비트 P, RW, US, PWT, PCD, A, D, 1, G, 3비트 Avail, 1비트 PAT, 8비트 Avail, 11비트 Base Address
	DWORD dwAttributeAndLowerBaseAddress;
	//8비트 Upper Base Address, 12 비트 Reserved, 11 비트 Avail, 1비트 EXB
	DWORD dwUpperBaseAddressAndEXB;
}PML4TEntry, PDPTEntry, PDEntry, PTEntry;
#pragma pack(pop)

void InitializePageTables();
void SetPageEntryData(PTEntry* pEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);
#endif