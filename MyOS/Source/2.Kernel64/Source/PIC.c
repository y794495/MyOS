#include "PIC.h"

void InitializePIC()
{
	//ICW1, ������ PIC �ʱ�ȭ,IC4 ��Ʈ 1
	OutPortByte(PIC_MASTER_PORT1, 0x11);
	//ICW2, ���ͷ�Ʈ ����(0x20)
	OutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

	//ICW3, �����̺� PIC ���� ��ġ
	OutPortByte(PIC_MASTER_PORT2, 0x04);
	//ICW4, uPM��Ʈ 1
	OutPortByte(PIC_MASTER_PORT2, 0x01);

	//ICW1, �����̺� PIC �ʱ�ȭ, IC4 1
	OutPortByte(PIC_SLAVE_PORT1, 0x11);
	//ICW2, ���ͷ�Ʈ����
	OutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);
	//ICW3, ������ PIC ��Ʈ�ѷ��� ����� ��ġ
	OutPortByte(PIC_SLAVE_PORT2, 0x02);
	//ICW4, uPM��Ʈ 1
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