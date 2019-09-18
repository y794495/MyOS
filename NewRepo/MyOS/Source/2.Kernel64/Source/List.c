#include "List.h"

void InitializeList(List* pstList)
{
	pstList->iItemCount = 0;
	pstList->pvHeader = NULL;
	pstList->pvTail = NULL;
}

int GetListCount(const List* pstList)
{
	return pstList->iItemCount;
}

void AddListToTail(List* pstList, void* pvItem)
{
	ListLink* pstLink;

	pstLink = (ListLink*)pvItem;
	pstLink->pvNext = NULL;

	if (pstList->pvHeader == NULL)
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;

		return;
	}

	pstLink = (ListLink*)pstList->pvTail;
	pstLink->pvNext = pvItem;

	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

void AddListToHeader(List* pstList, void* pvItem)
{
	ListLink* pstLink;

	pstLink = (ListLink*)pvItem;
	pstLink->pvNext = pstList->pvHeader;

	if (pstList->pvHeader == NULL)
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;

		return;
	}

	pstList->pvHeader = pvItem;
	pstList->iItemCount++;
}

void* RemoveList(List* pstList, QWORD qwID)
{
	ListLink* pstLink;
	ListLink* pstPreviousLink;

	pstPreviousLink = (ListLink*)pstList->pvHeader;
	for (pstLink = pstPreviousLink; pstLink != NULL; pstLink = pstLink->pvNext)
	{
		if (pstLink->qwID == qwID)
		{
			if ((pstLink == pstList->pvHeader) && (pstLink == pstList->pvTail))
			{
				pstList->pvHeader = NULL;
				pstList->pvTail = NULL;
			}
			else if (pstLink == pstList->pvHeader)
			{
				pstList->pvHeader == pstLink->pvNext;
			}
			else if (pstLink == pstList->pvTail)
			{
				pstList->pvTail = pstPreviousLink;
			}
			else
			{
				pstPreviousLink->pvNext = pstLink->pvNext;
			}

			pstList->iItemCount--;
			return pstLink;
		}
		pstPreviousLink = pstLink;
	}
	return NULL;
}


void* RemoveListFromHeader(List* pstList)
{
	ListLink* pstLink;

	if (pstList->iItemCount == 0)
	{
		return NULL;
	}

	pstLink = (ListLink*)pstList->pvHeader;
	return RemoveList(pstList, pstLink->qwID);
}

void* RemoveListFromTail(List* pstList)
{
	ListLink* pstLink;
	
	if (pstList->iItemCount == 0)
	{
		return NULL;
	}

	pstLink = (ListLink*)pstList->pvTail;
	return RemoveList(pstList, pstLink->qwID);
}

void* FindList(const List* pstList, QWORD qwID)
{
	ListLink* pstLink;

	for (pstLink = (ListLink*)pstList->pvHeader; pstLink != NULL; pstLink = pstLink->pvNext)
	{
		if (pstLink->qwID == qwID)
		{
			return pstLink;
		}
	}
	return NULL;
}

void* GetHeaderFromList(const List* pstList)
{
	return pstList->pvHeader;
}


void* GetTailFromList(const List* pstList)
{
	return pstList->pvTail;
}

void* GetNextFromList(const List* pstList, void* pstCurrent)
{
	ListLink* pstLink;

	pstLink = (ListLink*)pstCurrent;

	return pstLink->pvNext;
}