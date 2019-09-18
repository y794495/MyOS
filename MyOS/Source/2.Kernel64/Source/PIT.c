#include "PIT.h"

//PIT를 초기화
//카운터에 최대로 할당할수있는 크기 0x10000 (65536)
//컨트롤러에 0을 카운터에 전송하면 다음클록에서 최댓값인 0xFFFF로 변경한뒤 카운트 시작
void InitializePIT(WORD wCount, BOOL bPeriodic)
{
	//PIT 컨트롤 레지스터(포트 0x43)에 값을 초기화하여 카운트를 멈춘뒤에
	//모드 0에 바이너리 카운터로 설정
	//일정주기로 반복하는 타이머라면 모드2로 설정
	if (bPeriodic == TRUE)
	{
		//PIT 컨트롤 레지스터(포트 0x43)에 모드 2에 바이너리 카운터로 설정
		OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC);
	}
	else
	{
		OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_ONCE);
	}

	//카운터 0(포트 0x40)에 LSB-> MSB 순으로 카운터 초기값 설정
	OutPortByte(PIT_PORT_COUNTER0, wCount & 0xFF);
	OutPortByte(PIT_PORT_COUNTER0, wCount >> 8);
}

//카운터 0의 현재 값을 반환
WORD ReadCounter0()
{
	BYTE bHighByte, bLowByte;
	WORD wTemp = 0;

	//PIT 컨트롤 레지스터(포트 0x43)에 래치 커맨드를 전송하여 카운터 0에 있는 현재 값을 읽음.
	OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_LATCH);

	//카운터0 (포트0x40)에서 LSB->MSB 순으로 카운터 값을 읽음
	bLowByte = InPortByte(PIT_PORT_COUNTER0);
	bHighByte = InPortByte(PIT_PORT_COUNTER0);

	//상위 하위바이트 합쳐서 반환
	wTemp = bHighByte;
	wTemp = (wTemp << 8) | bLowByte;
	return wTemp;
}


//카운터 0을 직접 설정하여 일정시간 이상 대기
//함수를 호출하면 PIT 컨트롤러 설정이 바뀌므로 이후에 PIT 컨트롤러를 재설정해야함
//정확하게 측정하려면 함수 사용전에 인터럽트를 비활성화 하는 것이 좋음
//약 50ms까지 측정 가능
void WaitUsingDirectPIT(WORD wCount)
{
	WORD wLastCounter0;
	WORD wCurrentCounter0;

	InitializePIT(0, TRUE);

	//지금부터 wCount 이상 증가할 때까지 대기
	wLastCounter0 = ReadCounter0();
	while (1)
	{
		//현재 카운터0의 값을 반환
		wCurrentCounter0 = ReadCounter0();
		if (((wLastCounter0 - wCurrentCounter0) & 0xFFFF) >= wCount)
		{
			break;
		}
	}
}