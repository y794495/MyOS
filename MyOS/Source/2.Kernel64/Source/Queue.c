#include "Queue.h"

void InitializeQueue(Queue* pQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize)
{
	pQueue->iMaxDataCount = iMaxDataCount;
	pQueue->iDataSize = iDataSize;
	pQueue->pvQueueArray = pvQueueBuffer;

	pQueue->iPutIndex = 0;
	pQueue->iGetIndex = 0;
	pQueue->bLastOperationPut = FALSE;
}

BOOL IsQueueFull(const Queue* pQueue)
{
	if ((pQueue->iGetIndex == pQueue->iPutIndex) && (pQueue->bLastOperationPut == TRUE))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL IsQueueEmpty(const Queue* pQueue)
{
	if ((pQueue->iGetIndex == pQueue->iPutIndex) && (pQueue->bLastOperationPut == FALSE))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL PutQueue(Queue* pQueue, const void* pvData)
{
	if (IsQueueFull(pQueue) == TRUE)
	{
		return FALSE;
	}

	MemoryCopy((char*)pQueue->pvQueueArray + (pQueue->iDataSize * pQueue->iPutIndex), pvData, pQueue->iDataSize);
	pQueue->iPutIndex = (pQueue->iPutIndex + 1) % pQueue->iMaxDataCount;
	pQueue->bLastOperationPut = TRUE;
	return TRUE;
}

BOOL GetQueue(Queue* pQueue, void* pvData)
{
	if (IsQueueEmpty(pQueue) == TRUE)
	{
		return FALSE;
	}

	MemoryCopy(pvData, (char*)pQueue->pvQueueArray + (pQueue->iDataSize * pQueue->iGetIndex), pQueue->iDataSize);

	pQueue->iGetIndex = (pQueue->iGetIndex + 1) % pQueue->iMaxDataCount;
	pQueue->bLastOperationPut = FALSE;
	return TRUE;
}