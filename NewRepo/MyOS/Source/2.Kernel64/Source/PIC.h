#ifndef PIC_H
#define PIC_H

#include "Types.h"

#define PIC_MASTER_PORT1	0x20
#define PIC_MASTER_PORT2	0x21
#define PIC_SLAVE_PORT1		0xA0
#define PIC_SLAVE_PORT2		0xA1

#define PIC_IRQSTARTVECTOR	0x20

void InitializePIC(void);		//PIC를 초기화
void MaskPICInterrupt(WORD wIRQBitmask);//인터럽트를 마스크하여 해당 인터럽트가 발생하지 않게 처리
void SendEOIToPIC(int iIRQNumber);	//인터럽트 처리가 완료됬음을 전송


#endif