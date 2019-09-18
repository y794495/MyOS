#include "PIC.h"

void InitializePIC()
{
	//ICW1, 마스터 PIC 초기화,IC4 비트 1
	OutPortByte(PIC_MASTER_PORT1, 0x11);
	//ICW2, 인터럽트 벡터(0x20)
	OutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

	//ICW3, 슬레이브 PIC 연결 위치
	OutPortByte(PIC_MASTER_PORT2, 0x04);
	//ICW4, uPM비트 1
	OutPortByte(PIC_MASTER_PORT2, 0x01);

	//ICW1, 슬레이브 PIC 초기화, IC4 1
	OutPortByte(PIC_SLAVE_PORT1, 0x11);
	//ICW2, 인터럽트벡터
	OutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);
	//ICW3, 마스터 PIC 컨트롤러에 연결된 위치
	OutPortByte(PIC_SLAVE_PORT2, 0x02);
	//ICW4, uPM비트 1
	OutPortByte(PIC_SLAVE_PORT2, 0x01);
}

void MaskPICInterrupt(WORD wIRQBitmask)
{
	OutPortByte(PIC_MASTER_PORT2, (BYTE)wIRQBitmask);

	OutPortByte(PIC_SLAVE_PORT2, (BYTE)(wIRQBitmask >> 8));
}

void SendEOIToPIC(int iIRQNumber)
{
	OutPortByte(PIC_MASTER_PORT1, 0x20);

	if (iIRQNumber >= 8)
	{
		OutPortByte(PIC_SLAVE_PORT1, 0x20);
	}
}