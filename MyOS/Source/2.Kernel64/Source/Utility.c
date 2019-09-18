#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

volatile QWORD g_qwTickCount = 0;

void MemorySet(void* pvDestination, BYTE bData, int iSize)
{
	int i;

	for (i = 0; i < iSize; i++)
	{
		((char*)pvDestination)[i] = bData;
	}
}

int MemoryCopy(void* pvDestination, const void* pvSource, int iSize)
{
	int i;

	for (i = 0; i < iSize; i++)
	{
		((char*)pvDestination)[i] = ((char*)pvSource)[i];
	}
	return iSize;
}

int MemoryCompare(const void* pvDestination, const void* pvSource, int iSize)
{
	int i;
	char cTemp;

	for (i = 0; i < iSize; i++)
	{
		cTemp = ((char*)pvDestination)[i] - ((char*)pvSource)[i];
		if (cTemp != 0)
		{
			return (int)cTemp;
		}
	}
	return 0;
}

BOOL SetInterruptFlag(BOOL bEnableInterrupt)//인터럽트 플래그를 설정하고 이전의 인터럽트 플래그값을 반환
{
	QWORD qwRFLAGS;

	qwRFLAGS = ReadRFLAGS();

	if (bEnableInterrupt == TRUE)
	{
		EnableInterrupt();
	}
	else
	{
		DisableInterrupt();
	}

	if (qwRFLAGS & 0x0200)
	{
		return TRUE;
	}
	return FALSE;
}

int StrLen(const char* pcBuffer)
{
	int Length;
	for (Length = 0;; Length++)
	{
		if (pcBuffer[Length] == '\0')
		{
			break;
		}
	}
	return Length;
}

static gs_qwTotalRAMMBSize = 0;

void CheckTotalRAMSize()
{
	DWORD* pdwCurrentAddress;
	DWORD dwPreviousValue;

	pdwCurrentAddress = (DWORD*)0x4000000;
	while (1)
	{
		dwPreviousValue = *pdwCurrentAddress;
		*pdwCurrentAddress = 0x12345678;
		if (*pdwCurrentAddress != 0x12345678)
		{
			break;
		}
		*pdwCurrentAddress = dwPreviousValue;
		pdwCurrentAddress += (0x400000 / 4);
	}
	gs_qwTotalRAMMBSize = (QWORD)pdwCurrentAddress / 0x100000;
}

QWORD GetTotalRAMMBSize()
{
	return gs_qwTotalRAMMBSize;
}

long AToI(const char* pcBuffer, int iRadix)
{
	long lReturn;

	switch (iRadix)
	{
	case 16:
		lReturn = HexStringToQword(pcBuffer);
		break;
	case 10:
	default:
		lReturn = DecimalStringToLong(pcBuffer);
		break;
	}
	return lReturn;
}

QWORD HexStringToQword(const char* pcBuffer)
{
	QWORD qwValue = 0;
	int i;

	for (i = 0; pcBuffer[i] != '\0'; i++)
	{
		qwValue *= 16;
		if (('A' <= pcBuffer[i]) && (pcBuffer[i] <= 'Z'))
		{
			qwValue += (pcBuffer[i] - 'A') + 10;
		}
		else if (('a' <= pcBuffer[i]) && (pcBuffer[i] <= 'z'))
		{
			qwValue += (pcBuffer[i] - 'a') + 10;
		}
		else
		{
			qwValue += pcBuffer[i] - '0';
		}
	}
	return qwValue;
}

long DecimalStringToLong(const char* pcBuffer)
{
	long lValue = 0;
	int i;

	if (pcBuffer[0] == '-')
	{
		i = 1;
	}
	else
	{
		i = 0;
	}

	for (; pcBuffer[i] != '\0'; i++)
	{
		lValue *= 10;
		lValue += pcBuffer[i] - '0';
	}

	if (pcBuffer[0] == '-')
	{
		lValue = -lValue;
	}
	return lValue;
}

int IToA(long lValue, char* pcBuffer, int iRadix)
{
	int iReturn;

	switch (iRadix)
	{
	case 16:
		iReturn = HexToString(lValue, pcBuffer);
		break;
	case 10:
	default:
		iReturn = DecimalToString(lValue, pcBuffer);
		break;
	}
	return iReturn;
}

int HexToString(QWORD qwValue, char* pcBuffer)
{
	QWORD i;
	QWORD qwCurrentValue;

	if (qwValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	for (i = 0; qwValue > 0; i++)
	{
		qwCurrentValue = qwValue % 16;
		if (qwCurrentValue >= 10)
		{
			pcBuffer[i] = 'A' + (qwCurrentValue - 10);
		}
		else
		{
			pcBuffer[i] = '0' + qwCurrentValue;
		}

		qwValue = qwValue / 16;
	}
	pcBuffer[i] = '\0';

	ReverseString(pcBuffer);
	return i;
}

int DecimalToString(long lValue, char* pcBuffer)
{
	long i;

	if (lValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	if (lValue < 0)
	{
		i = 1;
		pcBuffer[0] = '-';
		lValue = -lValue;
	}
	else
	{
		i = 0;
	}

	for (; lValue > 0; i++)
	{
		pcBuffer[i] = '0' + lValue % 10;
		lValue = lValue / 10;
	}
	pcBuffer[i] = '\0';

	if (pcBuffer[0] == '-')
	{
		ReverseString(&(pcBuffer[1]));
	}
	else
	{
		ReverseString(pcBuffer);
	}

	return i;
}

void ReverseString(char* pcBuffer)
{
	int iLength;
	int i;
	char cTemp;

	iLength = StrLen(pcBuffer);
	for (i = 0; i < iLength / 2; i++)
	{
		cTemp = pcBuffer[i];
		pcBuffer[i] = pcBuffer[iLength - 1 - i];
		pcBuffer[iLength - 1 - i] = cTemp;
	}
}

int SPrintf(char* pcBuffer, const char* pcFormatString, ...)
{
	va_list ap;
	int iReturn;

	va_start(ap, pcFormatString);
	iReturn = VSPrintf(pcBuffer, pcFormatString, ap);
	va_end(ap);

	return iReturn;
}

int VSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap)
{
	QWORD i, j;
	int iBufferIndex = 0;
	int iFormatLength, iCopyLength;
	char* pcCopyString;
	QWORD qwValue;
	int iValue;

	iFormatLength = StrLen(pcFormatString);//포맷 문자열의 길이를 읽어서 문자열의 길이만큼 데이터를 출력 버퍼에 출력
	for (i = 0; i < iFormatLength; i++)
	{
		if (pcFormatString[i] == '%')//%로 시작하면 데이터 타입 문자로 처리
		{
			i++;//%다음 문자로 이동
			switch (pcFormatString[i])
			{
			case 's'://문자열 타입
				pcCopyString = (char*)(va_arg(ap, char*));
				iCopyLength = StrLen(pcCopyString);
				MemoryCopy(pcBuffer + iBufferIndex, pcCopyString, iCopyLength);
				iBufferIndex += iCopyLength;
				break;

			case 'c'://문자
				pcBuffer[iBufferIndex] = (char)(va_arg(ap, int));
				iBufferIndex++;
				break;

			case 'd'://정수
			case 'i':
				iValue = (int)(va_arg(ap, int));
				iBufferIndex += IToA(iValue, pcBuffer + iBufferIndex, 10);
				break;

			case 'x'://4바이트 Hex
				qwValue = (DWORD)(va_arg(ap, DWORD)) & 0xFFFFFFFF;
				iBufferIndex += IToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

			case 'q':
			case 'Q':
			case 'p':
				qwValue = (QWORD)(va_arg(ap, QWORD));
				iBufferIndex += IToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

			default:
				pcBuffer[iBufferIndex] = pcFormatString[i];
				iBufferIndex++;
				break;
			}
		}
		else
		{
			pcBuffer[iBufferIndex] = pcFormatString[i];
			iBufferIndex++;
		}
	}

	pcBuffer[iBufferIndex] = '\0';
	return iBufferIndex;
}

QWORD GetTickCount()
{
	return g_qwTickCount;
}