#ifndef List_H
#define List_H

#include "Types.h"

#pragma pack(push,1)

typedef struct ListLinkStruct
{
	void* pvNext;
	QWORD qwID;
}ListLink;

/*
struct ListItemExampleStruct
{
	ListLink stLink;

	int iData1;
	char cData2;
};
*/

typedef struct ListManagerStruct
{
	int iItemCount;
	void* pvHeader;
	void* pvTail;
}List;

#pragma pack(pop)

void InitializeList(List* pstList);
int GetListCount(const List* pstList);
void AddListToTail(List* pstList, void* pvItem);
void AddListToHeader(List* pstList, void* pvItem);
void* RemoveList(List* pstList, QWORD qwID);
void* RemoveListFromHeader(List* pstList);
void* RemoveListFromTail(List* pstList);
void* FindList(const List* pstList, QWORD qwID);
void* GetHeaderFromList(const List* pstList);
void* GetTailFromList(const List* pstList);
void* GetNextFromList(const List* pstList, void* pstCurrent);

#endif