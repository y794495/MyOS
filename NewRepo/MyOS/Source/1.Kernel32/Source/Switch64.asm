[bits 32]

global ReadCPUID, SwitchAndExecute64bitKernel	;�Լ� �̸� ��������

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

	;EAX �������� ������ CPUID ��ɾ� ����
	mov eax, dword[ebp+8]	;�Ķ���� 1�� EAX �������Ϳ� ����
	cpuid					;EAX�� 0�϶�
							;EAX(�ش� ���μ����� �����ϴ� �⺻ CPUID ���� �Է��� �ִ밪)
							;EBX EDX ECX ����, ��������Ʈ���� ��������Ʈ�� ������ 12����Ʈ ������ �̸��� �����
							;������ 'GenuineIntel' AMD�� 'AuthenticAMD'�� ǥ��

							;EAX�� 0x80000001�϶�
							;EAX, EBX�� �����縶�� �ٸ�.
							;ECX :��Ʈ 0-64��Ʈ ��忡�� LAHF/SAHF ��� ���� ����
							;    :�� �� ������ ��Ʈ - �����縶�� ����
							;EDX :��Ʈ 11-64��Ʈ ��忡�� SYSCALL/SYSRET ��� ���� ����
							;    :��Ʈ 20 - Execute Disable ��Ʈ ���� ����
							;    :��Ʈ 29 - 64 ��Ʈ ���� ����
							;    :�� �� ������ ��Ʈ - �����縶�� ����
	;*pdwEAX
	mov esi, dword[ebp+12]	;�Ķ���� 2�� ESI �������Ϳ� ����
	mov dword[esi],eax		;EAX �������� ���� �����Ͱ� ����Ű�� �ּҿ� ����

	;*pdwEBX
	mov esi, dword[ebp+16]	;�Ķ���� 3�� ESI �������Ϳ� ����
	mov dword[esi], ebx		;EBX �������� ���� �����Ͱ� ����Ű�� �ּҿ� ����

	;*pdwECX
	mov esi, dword[ebp+20]	;�Ķ���� 4�� ESI �������Ϳ� ����
	mov dword[esi], ecx		;ECX �������� ���� �����Ͱ� ����Ű�� �ּҿ� ����

	;*pdwEDX
	mov esi, dword[ebp+24]	;�Ķ���� 5�� ESI �������Ϳ� ����
	mov dword[esi],edx		;EDX �������� ���� �����Ͱ� ����Ű�� �ּҿ� ����

	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret

SwitchAndExecute64bitKernel:
	mov eax, cr4		;CR4��Ʈ�� ���������� ���� EAX �������Ϳ� ����
	or eax, 0x20		;PAE��Ʈ�� 1�� ����
	mov cr4, eax		;PAE��Ʈ�� 1�� ������ ���� CR4 ��Ʈ�� �������Ϳ� ����
	
	;CR3 ��Ʈ�� �������Ϳ� PML4 ���̺��� ��巹���� ĳ�� Ȱ��ȭ
	mov eax, 0x100000	;EAX�������Ϳ� PML4 ���̺��� �����ϴ� 0x100000�� ����
	mov cr3, eax		;CR3�������Ϳ� �� ���̺� �ּ� ����

	mov ecx, 0xC0000080	;IA32_EFER(Extended Feature Enable Register) MSR ���������� ��巹���� ����
						;63~12 �����縶�� ����
						;11 NXE(No-Execute Enable)
						;10 LMA(Long Mode Active) ���� �������� ��尡 IA-32e ������� ���θ� ǥ��
						;9 �����
						;8 LME(Long Mode Enable) 1�� �����ϸ� IA-32e��带 Ȱ��ȭ��
						;7~1 �����
						;0 SCE(System Call Enable) SYSCALL SYSRET ��ɾ ������� ���θ� �ǹ� 1�̸� ���

	rdmsr				;MSR(Model-Specific Register)(���μ��� �𵨿����� Ư���ϰ� ���ǵ� ��������) �������͸� �д´�.
						

	or eax, 0x0100		;EAX �������Ϳ� ����� IA32_EFER MSR ���������� ���� ��Ʈ�� 8����Ʈ�� Ȱ��ȭ

	wrmsr
	
	mov eax, cr0		;EAX �������Ϳ� CR0 ��Ʈ�� �������͸� ����
	or eax, 0xE000000E	;NW ��Ʈ(29), CD ��Ʈ(30), PG��Ʈ(31) �� 1�� ����
	xor eax, 0x60000000	;NW ��Ʈ, CD��Ʈ�� XOR�Ͽ� 0���� ����
	mov cr0, eax		;NW ��Ʈ=0, CD��Ʈ 0, PG��Ʈ 1�� ���� cr0 ��Ʈ�� �������Ϳ� ����
	
	jmp 0x08:0x200000

	jmp $