#ifndef __TYPES_H
#define __TYPES_H

#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned int
#define QWORD	unsigned long
#define BOOL	unsigned char

#define TRUE	1
#define FALSE	0
#define NULL	0

#pragma pack(push, 1)

typedef struct VideoStruct
{
	BYTE Charactor;
	BYTE Attribute;
}VideoStruct;

#pragma pack(pop)

#endif