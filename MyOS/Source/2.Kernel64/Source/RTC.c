#include "RTC.h"

//CMOS �޸𸮿��� RTC ��Ʈ�ѷ��� ������ ���� �ð��� ����
void ReadRTCTime(BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond)
{
	BYTE bData;

	//CMOS �޸� ��巹�� ��������(��Ʈ0x70)�� �ð��� �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_HOUR);
	//CMOS ������ ��������(��Ʈ0x71)���� �ð��� ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbHour = RTC_BCDTOBINARY(bData);

	//CMOS �޸� ��巹�� ��������(��Ʈ0x70)�� ���� �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE);
	//CMOS ������ ��������(��Ʈ 0x71)���� ���� ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbMinute = RTC_BCDTOBINARY(bData);

	//CMOS �޸� ��巹�� ��������(��Ʈ0x70)�� �ʸ� �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_SECOND);
	//CMOS ������ ��������(��Ʈ0x71)���� �ʸ� ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbSecond = RTC_BCDTOBINARY(bData);
}

//CMOS �޸𸮿��� RTC ��Ʈ�ѷ��� ������ ���� ���ڸ� ����
void ReadRTCDate(WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, BYTE* pbDayOfWeek)
{
	BYTE bData;

	//CMOS �޸� ��巹�� ��������(��Ʈ 0x70)�� ������ �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_YEAR);
	//CMOS ������ ��������(��Ʈ 0x71)���� ������ ����
	bData = InPortByte(RTC_CMOSDATA);
	*pwYear = RTC_BCDTOBINARY(bData) + 2000;

	//CMOS �޸� ��巹�� ��������(��Ʈ 0x70)�� ���� �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_MONTH);
	//CMOS ������ ��������(��Ʈ 0x71)���� ���� ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbMonth = RTC_BCDTOBINARY(bData);

	//CMOS �޸� ��巹�� ��������(��Ʈ 0x70)�� ���� �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH);
	//CMOS ������ ��������(��Ʈ 0x71)���� ���� ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbDayOfMonth = RTC_BCDTOBINARY(bData);

	//CMOS �޸� ��巹�� ��������(��Ʈ 0x70)�� ������ �����ϴ� �������� ����
	OutPortByte(RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK);
	//CMOS ������ ��������(��Ʈ 0x71)���� ������ ����
	bData = InPortByte(RTC_CMOSDATA);
	*pbDayOfWeek = RTC_BCDTOBINARY(bData);
}

//���� ���� �̿��ؼ� �ش� ������ ���ڿ��� ��ȯ
char* ConvertDayOfWeekToString(BYTE bDayOfWeek)
{
	static char* vpcDayOfWeekString[8] = { "Error","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

	if (bDayOfWeek >= 8)
	{
		return vpcDayOfWeekString[0];
	}

	return vpcDayOfWeekString[bDayOfWeek];
}