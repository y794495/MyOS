#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "Queue.h"

//읽기 명령 수행후 잘받았다는 신호 (0xFA) 확인후 버퍼 비었는지 확인후 쓰기 수행
//읽기(쓰기):상태 레지스터 0x64(컨트롤 레지스터), 출력 버퍼 레지스터 0x60(입력 버퍼)
BOOL IsOutputBufferFull()//출력 버퍼 체크
{
    if(InPortByte(0x64) & 0x01)//키보드가 보낸 데이터가 남아있는지 확인
    {
        return TRUE;
    }
	return FALSE;
}

BOOL IsInputBufferFull()//입력 버퍼 체크
{
    if(InPortByte(0x64) & 0x02)//프로세서가 보낸 데이터가 아직 남아있는지 확인
    {
        return TRUE;
    }
    return FALSE;
}

BOOL WaitForACKAndPutOtherScanCode()
{
	int i, j;
	BYTE bData;
	BOOL bResult = FALSE;
	for (j = 0; j < 100; j++)
	{
		for (i = 0; i < 0xFFFF; i++)
		{
			if (IsOutputBufferFull() == TRUE)
			{
				break;
			}
		}
		bData = InPortByte(0x60);
		if (bData == 0xFA)
		{
			bResult = TRUE;
			break;
		}
		else
		{
			ConvertScanCodeAndPutQueue(bData);
		}
	}
	return bResult;
}


BOOL ActivateKeyboard()
{
    int i;
    int j;
	BOOL bPreviousInterrupt;
	BOOL bResult;

	bPreviousInterrupt = SetInterruptFlag(FALSE);
    OutPortByte(0x64, 0xAE);//아웃포트 확인(0x64(키보드 상태 레지스터 포트)) 키보드 디바이스 활성화
	
    //0xFFFF만큼 루프를 수행할 시간이면 충분히 커맨드 전송가능함
    //0xFFFF 루프를 수행한 이후에도 입력 버퍼(포트 0x60이 비지않으면 무시하고 전송)
    for(i=0;i<0xFFFF;i++)
    {
        if(IsInputBufferFull()==FALSE)//입력버퍼가 비어있다 즉. 키보드가 읽었다.
        {
			break;
        }
    }
    OutPortByte(0x60, 0xF4);//키보드활성화 커맨드를 0x60포트(키보드 데이터 레지스터 포트)에 전송
	
	bResult = WaitForACKAndPutOtherScanCode();//ACK가 올때까지 대기

	SetInterruptFlag(bPreviousInterrupt);//이전 인터럽트 상태 복원
    return bResult;
}

BYTE GetKeyboardScanCode()
{
    while(IsOutputBufferFull()==FALSE)
    {
        ;
    }
    return InPortByte(0x60);
}

//키보드 상태 LED 제어
BOOL ChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn)
{
    int i, j;
	BOOL bPreviousInterrupt;
	BOOL bResult;
	BYTE bData;

	bPreviousInterrupt = SetInterruptFlag(FALSE);

    //키보드에 LED 변경 커맨드 전송하고 커맨드가 처리될 때까지 대기
    for(i = 0; i < 0xFFFF; i++)
    {
        if(IsInputBufferFull()==FALSE)
        {
            break;
        }
    }

    //키보드 데이터 레지스터 포트(0x60)에 LED 제어 전송
    OutPortByte(0x60, 0xED);
    for(i = 0; i < 0xFFFF; i++)
    {
        if(IsInputBufferFull()==FALSE)
        {
            break;
        }
    }

	bResult = WaitForACKAndPutOtherScanCode();

	if (bResult == FALSE)
	{
		SetInterruptFlag(bPreviousInterrupt);
		return FALSE;
	}


    OutPortByte(0x60, (bCapsLockOn << 2) | (bNumLockOn << 1) | bScrollLockOn);//데이터 레지스터에 쓴다.
    for(i=0;i<0xFFFF;i++)
    {
        if(IsInputBufferFull()==FALSE)//전송될때까지 대기
        {
            break;
        }
    }

	bResult = WaitForACKAndPutOtherScanCode();

	SetInterruptFlag(bPreviousInterrupt);//이전 인터럽트 상태 복원
	return bResult;
}

void EnableA20Gate()
{
    BYTE bOutputPortData;
    int i;

    OutPortByte(0x64,0xD0);//컨트롤 레지스터에 키보드 컨트롤러의 출력 포트 값을 읽는 커맨드(0xD0)전송

    for(i=0;i<0xFFFF;i++)
    {
        if(IsOutputBufferFull()==TRUE)//출력 버퍼가 찼으면
        {
            break;
        }
    }

    bOutputPortData=InPortByte(0x60);//출력 포트에 수신된 키보드 컨트롤러의 출력 포트 값 읽음

    bOutputPortData|=0x01;//A20 게이트 비트 설정

    for(i=0;i<0xFFFF;i++)//입력 버퍼에 데이터가 비어있으면 출력 포트에 값을 쓰는 커맨드와 출력 포트 데이터 전송
    {
        if(IsInputBufferFull()==FALSE)//입력 버퍼가 비었으면 커맨드 전송가능
        {
            break;
        }

        OutPortByte(0x64, 0xD1);//커맨드 레지스터에 출력 포트 설정 커맨드를 전달

        OutPortByte(0x60, bOutputPortData);//입력 버퍼에 A20 게이트 비트가 1로 설정된 값을 전달
    }
}

void Reboot()
{
    int i;

    for(i=0;i<0xFFFF;i++)
    {
        if(IsInputBufferFull()==FALSE)
        {
            break;
        }
    }

    OutPortByte(0x64, 0xD1);

    OutPortByte(0x60, 0x00);

    while(1);
}

static KeyboardManager gs_stKeyboardManager={0, };

static Queue gs_stKeyQueue;//키를 저장하는 큐와 버퍼 정의
static KeyData gs_vstKeyQueueBuffer[KEY_MAXQUEUECOUNT];

//키보드에서 눌린값을 인덱스로하여 ASCII 코드로 매핑
static KeyMappingEntry gs_vstKeyMappingTable[KEY_MAPPINGTABLE_MAXCOUNT]={
    {KEY_NONE,      KEY_NONE},
    {KEY_ESC,       KEY_ESC},
    {'1',           '!'},
    {'2',           '@'},
    {'3',           '#'},
    {'4',           '$'},
    {'5',           '%'},
    {'6',           '^'},
    {'7',           '&'},
    {'8',           '*'},
    {'9',           '('},
    {'0',           ')'},
    {'-',           '_'},
    {'=',           '+'},
    {KEY_BACKSPACE, KEY_BACKSPACE},
    {KEY_TAB,       KEY_TAB},
    {'q',           'Q'},
    {'w',           'W'},
    {'e',           'E'},
    {'r',           'R'},
    {'t',           'T'},
    {'y',           'Y'},
    {'u',           'U'},
    {'i',           'I'},
    {'o',           'O'},
    {'p',           'P'},
    {'[',           '{'},
    {']',           '}'},
    {'\n',          '\n'},
    {KEY_CTRL,      KEY_CTRL},
    {'a',           'A'},
    {'s',           'S'},
    {'d',           'D'},
    {'f',           'F'},
    {'g',           'G'},
    {'h',           'H'},
    {'j',           'J'},
    {'k',           'K'},
    {'l',           'L'},
    {';',           ':'},
    {'\'',           '\"'},
    {'`',           '~'},
    {KEY_LSHIFT,    KEY_LSHIFT},
    {'\\',          '\\'},
    {'z',           'Z'},
    {'x',           'X'},
    {'c',           'C'},
    {'v',           'V'},
    {'b',           'B'},
    {'n',           'N'},
    {'m',           'M'},
    {',',           '<'},
    {'.',           '>'},
    {'/',           '?'},
    {KEY_RSHIFT,    KEY_RSHIFT},
    {'*',           '*'},
    {KEY_LALT,      KEY_LALT},
    {' ',           ' '},
    {KEY_CAPSLOCK,  KEY_CAPSLOCK},
    {KEY_F1,        KEY_F1},
    {KEY_F2,        KEY_F2},
    {KEY_F3,        KEY_F3},
    {KEY_F4,        KEY_F4},
    {KEY_F5,        KEY_F5},
    {KEY_F6,        KEY_F6},
    {KEY_F7,        KEY_F7},
    {KEY_F8,        KEY_F8},
    {KEY_F9,        KEY_F9},
    {KEY_F10,       KEY_F10},
    {KEY_NUMLOCK,   KEY_NUMLOCK},
    {KEY_SCROLLLOCK,KEY_SCROLLLOCK},
    {KEY_HOME,      '7'},
    {KEY_UP,        '8'},
    {KEY_PAGEUP,    '9'},
    {'-',           '-'},
    {KEY_LEFT,      '4'},
    {KEY_CENTER,    '5'},
    {KEY_RIGHT,     '6'},
    {'+',           '+'},
    {KEY_END,       '1'},
    {KEY_DOWN,      '2'},
    {KEY_PAGEDOWN,  '3'},
    {KEY_INS,       '0'},
    {KEY_DEL,       '.'},
    {KEY_NONE,      KEY_NONE},
    {KEY_NONE,      KEY_NONE},
    {KEY_NONE,      KEY_NONE},
    {KEY_F11,       KEY_F11},
    {KEY_F12,       KEY_F12}
};

BOOL IsAlphabetScanCode(BYTE bScanCode)//스캔코드가 알파벳 범위인지 여부를 반환
{
    if(('a'<=gs_vstKeyMappingTable[bScanCode].bNormalCode)&&(gs_vstKeyMappingTable[bScanCode].bNormalCode<='z'))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}

BOOL IsNumberOrSymbolScanCode(BYTE bScanCode)//숫자 또는 기호 범위인지 여부를 반환
{
    if((2<=bScanCode)&&(bScanCode<=53)&&IsAlphabetScanCode(bScanCode)==FALSE)// !~/까지
    {
        return TRUE;
    }
    return FALSE;
}

BOOL IsNumberPadScanCode(BYTE bScanCode)//숫자패드 범위인지 여부를 반환
{
    if((71<=bScanCode)&&(bScanCode<=83))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL IsUseCombinedCode(BOOL bScanCode)//조합된 키 값을 사용해야 하는지 여부를 반환
{
    BYTE bDownScanCode;
    BOOL bUseCombinedKey;

    bDownScanCode = bScanCode & 0x7F;
    if(IsAlphabetScanCode(bDownScanCode) == TRUE)
    {
        if(gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn)
        {
			bUseCombinedKey = TRUE;
        }
        else
        {
			bUseCombinedKey = FALSE;
        }
    }
    else if(IsNumberOrSymbolScanCode(bDownScanCode) == TRUE)
    {
        if(gs_stKeyboardManager.bShiftDown == TRUE)
        {
			bUseCombinedKey = TRUE;
        }
        else
        {
			bUseCombinedKey = FALSE;
        }   
    }
    else if((IsNumberPadScanCode(bDownScanCode) == TRUE) && (gs_stKeyboardManager.bExtendedCodeIn == FALSE))
    {
        if(gs_stKeyboardManager.bNumLockOn == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
        
    }
    return bUseCombinedKey;
}

void UpdateCombinationKeyStatusAndLED(BYTE bScanCode)//조합 키의 상태 갱신, LED 상태 동기화
{
    BOOL bDown;
    BYTE bDownScanCode;
	BOOL bLEDStatusChanged = FALSE;

    if(bScanCode & 0x80)
    {
		bDown = FALSE;
		bDownScanCode = bScanCode & 0x7F;
    }
    else
    {
		bDown = TRUE;
        bDownScanCode = bScanCode & 0x7F;
    }
    
    if((bDownScanCode == 42) || (bDownScanCode == 54))
    {
		gs_stKeyboardManager.bShiftDown = bDown;
    }
    else if((bDownScanCode == 58) && (bDown == TRUE))
    {
		gs_stKeyboardManager.bCapsLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
    }
    else if((bDownScanCode == 69) && (bDown == TRUE))
    {
		gs_stKeyboardManager.bNumLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
    }
    else if((bDownScanCode == 70) && (bDown == TRUE))
    {
        gs_stKeyboardManager.bScrollLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
    }

    if(bLEDStatusChanged == TRUE)
    {
		ChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn, gs_stKeyboardManager.bNumLockOn, gs_stKeyboardManager.bScrollLockOn);
    }
}

BOOL ConvertScanCodeToASCIICode(BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags)
{
	BOOL bUseCombinedKey;

	if (gs_stKeyboardManager.iSkipCountForPause > 0)
	{
		gs_stKeyboardManager.iSkipCountForPause--;
		return FALSE;
	}

	if (bScanCode == 0xE1)//PAUSE 키 처리
	{
		*pbASCIICode = KEY_PAUSE;
		*pbFlags = KEY_FLAGS_DOWN;
		gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
		return TRUE;
	}
	else if (bScanCode == 0xE0)//Enter부터 Delete 까지(확장코드)
	{
		gs_stKeyboardManager.bExtendedCodeIn = TRUE;
		return FALSE;
	}
	
	bUseCombinedKey = IsUseCombinedCode(bScanCode);

	if (bUseCombinedKey == TRUE)
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
	}
	else
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
	}

	if (gs_stKeyboardManager.bExtendedCodeIn == TRUE)
	{
		*pbFlags = KEY_FLAGS_EXTENDEDKEY;
		gs_stKeyboardManager.bExtendedCodeIn = FALSE;
	}
	else
	{
		*pbFlags = 0;
	}

	if ((bScanCode & 0x80) == 0)
	{
		*pbFlags |= KEY_FLAGS_DOWN;
	}

	UpdateCombinationKeyStatusAndLED(bScanCode);
	return TRUE;
}

BOOL InitializeKeyboard()//키보드 초기화
{
	InitializeQueue(&gs_stKeyQueue, gs_vstKeyQueueBuffer, KEY_MAXQUEUECOUNT, sizeof(KeyData));//큐 초기화

	return ActivateKeyboard();//키보드 활성화
}

BOOL ConvertScanCodeAndPutQueue(BYTE bScanCode)//스캔 코드를 내부적으로 사용하는 키 데이터로 바꾼 후 키 큐에 삽입
{
	KeyData Data;
	BOOL bResult = FALSE;
	BOOL bPreviousInterrupt;

	Data.bScanCode = bScanCode;//스캔 코드를 키 데이터에 삽입

	if (ConvertScanCodeToASCIICode(bScanCode, &(Data.bASCIICode), &(Data.bFlags)) == TRUE)//스캔 코드를 ASCII 코드와 키 상태로 변환하여 키 데이터에 삽입
	{
		bPreviousInterrupt = SetInterruptFlag(FALSE);//인터럽트 불가
		bResult = PutQueue(&gs_stKeyQueue, &Data);//키 큐에 삽입
		SetInterruptFlag(bPreviousInterrupt);//이전 인터럽트 플래그 복원
	}

	return bResult;
}

BOOL GetKeyFromKeyQueue(KeyData* pData)
{
	BOOL bResult;
	BOOL bPreviousInterrupt;

	if (IsQueueEmpty(&gs_stKeyQueue) == TRUE)
	{
		return FALSE;
	}

	bPreviousInterrupt = SetInterruptFlag(FALSE);//인터럽트 불가

	bResult = GetQueue(&gs_stKeyQueue, pData);//키 큐에서 데이터를 제거

	SetInterruptFlag(bPreviousInterrupt);//이전 인터럽트 플래그 복원
	return bResult;
}
