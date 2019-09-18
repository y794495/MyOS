#ifndef Queue_H
#define Queue_H
#include "Types.h"
	
#pragma pack(push, 1)
typedef struct QueueManagerStruct
{
	int iDataSize;
	int iMaxDataCount;

	void* pvQueueArray;
	int iPutIndex;
	int iGetIndex;

	BOOL bLastOperationPut;
}Queue;

#pragma pack(pop)

void InitializeQueue(Queue* pQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize);
BOOL IsQueueFull(const Queue* pQueue);
BOOL IsQueueEmpty(const Queue* pQueue);
BOOL PutQueue(Queue* pQueue, const void* pvData);
BOOL GetQueue(Queue* pQueue, void* pvData);
#endif