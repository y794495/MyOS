#include "PIT.h"

//PIT�� �ʱ�ȭ
//ī���Ϳ� �ִ�� �Ҵ��Ҽ��ִ� ũ�� 0x10000 (65536)
//��Ʈ�ѷ��� 0�� ī���Ϳ� �����ϸ� ����Ŭ�Ͽ��� �ִ��� 0xFFFF�� �����ѵ� ī��Ʈ ����
void InitializePIT(WORD wCount, BOOL bPeriodic)
{
	//PIT ��Ʈ�� ��������(��Ʈ 0x43)�� ���� �ʱ�ȭ�Ͽ� ī��Ʈ�� ����ڿ�
	//��� 0�� ���̳ʸ� ī���ͷ� ����
	//�����ֱ�� �ݺ��ϴ� Ÿ�̸Ӷ�� ���2�� ����
	if (bPeriodic == TRUE)
	{
		//PIT ��Ʈ�� ��������(��Ʈ 0x43)�� ��� 2�� ���̳ʸ� ī���ͷ� ����
		OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC);
	}
	else
	{
		OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_ONCE);
	}

	//ī���� 0(��Ʈ 0x40)�� LSB-> MSB ������ ī���� �ʱⰪ ����
	OutPortByte(PIT_PORT_COUNTER0, wCount & 0xFF);
	OutPortByte(PIT_PORT_COUNTER0, wCount >> 8);
}

//ī���� 0�� ���� ���� ��ȯ
WORD ReadCounter0()
{
	BYTE bHighByte, bLowByte;
	WORD wTemp = 0;

	//PIT ��Ʈ�� ��������(��Ʈ 0x43)�� ��ġ Ŀ�ǵ带 �����Ͽ� ī���� 0�� �ִ� ���� ���� ����.
	OutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_LATCH);

	//ī����0 (��Ʈ0x40)���� LSB->MSB ������ ī���� ���� ����
	bLowByte = InPortByte(PIT_PORT_COUNTER0);
	bHighByte = InPortByte(PIT_PORT_COUNTER0);

	//���� ��������Ʈ ���ļ� ��ȯ
	wTemp = bHighByte;
	wTemp = (wTemp << 8) | bLowByte;
	return wTemp;
}


//ī���� 0�� ���� �����Ͽ� �����ð� �̻� ���
//�Լ��� ȣ���ϸ� PIT ��Ʈ�ѷ� ������ �ٲ�Ƿ� ���Ŀ� PIT ��Ʈ�ѷ��� �缳���ؾ���
//��Ȯ�ϰ� �����Ϸ��� �Լ� ������� ���ͷ�Ʈ�� ��Ȱ��ȭ �ϴ� ���� ����
//�� 50ms���� ���� ����
void WaitUsingDirectPIT(WORD wCount)
{
	WORD wLastCounter0;
	WORD wCurrentCounter0;

	InitializePIT(0, TRUE);

	//���ݺ��� wCount �̻� ������ ������ ���
	wLastCounter0 = ReadCounter0();
	while (1)
	{
		//���� ī����0�� ���� ��ȯ
		wCurrentCounter0 = ReadCounter0();
		if (((wLastCounter0 - wCurrentCounter0) & 0xFFFF) >= wCount)
		{
			break;
		}
	}
}