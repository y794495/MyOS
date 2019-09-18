[bits 32]

global ReadCPUID, SwitchAndExecute64bitKernel	;함수 이름 내보내기

SECTION .text

;PARAM: DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX
ReadCPUID:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push ecx
	push edx
	push esi

	;EAX 레지스터 값으로 CPUID 명령어 실행
	mov eax, dword[ebp+8]	;파라미터 1을 EAX 레지스터에 저장
	cpuid					;EAX가 0일때
							;EAX(해당 프로세서가 지원하는 기본 CPUID 정보 입력의 최대값)
							;EBX EDX ECX 순서, 하위바이트에서 상위바이트의 순서로 12바이트 제조사 이름이 저장됨
							;인텔은 'GenuineIntel' AMD는 'AuthenticAMD'로 표시

							;EAX가 0x80000001일때
							;EAX, EBX는 제조사마다 다름.
							;ECX :비트 0-64비트 모드에서 LAHF/SAHF 명령 지원 여부
							;    :그 외 나머지 비트 - 제조사마다 차이
							;EDX :비트 11-64비트 모드에서 SYSCALL/SYSRET 명령 지원 여부
							;    :비트 20 - Execute Disable 비트 지원 여부
							;    :비트 29 - 64 비트 지원 여부
							;    :그 외 나머지 비트 - 제조사마다 차이
	;*pdwEAX
	mov esi, dword[ebp+12]	;파라미터 2를 ESI 레지스터에 저장
	mov dword[esi],eax		;EAX 레지스터 값을 포인터가 가리키는 주소에 저장

	;*pdwEBX
	mov esi, dword[ebp+16]	;파라미터 3을 ESI 레지스터에 저장
	mov dword[esi], ebx		;EBX 레지스터 값을 포인터가 가리키는 주소에 저장

	;*pdwECX
	mov esi, dword[ebp+20]	;파라미터 4를 ESI 레지스터에 저장
	mov dword[esi], ecx		;ECX 레지스터 값을 포인터가 가리키는 주소에 저장

	;*pdwEDX
	mov esi, dword[ebp+24]	;파라미터 5를 ESI 레지스터에 저장
	mov dword[esi],edx		;EDX 레지스터 값을 포인터가 가리키는 주소에 저장

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret

SwitchAndExecute64bitKernel:
	mov eax, cr4		;CR4컨트롤 레지스터의 값을 EAX 레지스터에 저장
	or eax, 0x20		;PAE비트를 1로 설정
	mov cr4, eax		;PAE비트가 1로 설정된 값을 CR4 컨트롤 레지스터에 저장
	
	;CR3 컨트롤 레지스터에 PML4 테이블의 어드레스와 캐시 활성화
	mov eax, 0x100000	;EAX레지스터에 PML4 테이블이 존재하는 0x100000를 저장
	mov cr3, eax		;CR3레지스터에 그 테이블 주소 저장

	mov ecx, 0xC0000080	;IA32_EFER(Extended Feature Enable Register) MSR 레지스터의 어드레스를 저장
						;63~12 제조사마다 차이
						;11 NXE(No-Execute Enable)
						;10 LMA(Long Mode Active) 현재 동작중인 모드가 IA-32e 모드인지 여부를 표시
						;9 예약됨
						;8 LME(Long Mode Enable) 1로 설정하면 IA-32e모드를 활성화함
						;7~1 예약됨
						;0 SCE(System Call Enable) SYSCALL SYSRET 명령어를 사용할지 여부를 의미 1이면 사용

	rdmsr				;MSR(Model-Specific Register)(프로세서 모델에따라 특수하게 정의된 레지스터) 레지스터를 읽는다.
						

	or eax, 0x0100		;EAX 레지스터에 저장된 IA32_EFER MSR 레지스터의 하위 비트의 8번비트를 활성화

	wrmsr
	
	mov eax, cr0		;EAX 레지스터에 CR0 컨트롤 레지스터를 저장
	or eax, 0xE000000E	;NW 비트(29), CD 비트(30), PG비트(31) 를 1로 설정
	xor eax, 0x60000000	;NW 비트, CD비트를 XOR하여 0으로 설정
	mov cr0, eax		;NW 비트=0, CD비트 0, PG비트 1인 값을 cr0 컨트롤 레지스터에 저장
	
	jmp 0x08:0x200000

	jmp $