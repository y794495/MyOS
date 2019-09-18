[org 0x00]
[bits 16]

SECTION .text

start:
	mov ax, 0x1000
	mov ds, ax
	mov es, ax
	
	mov ax, 0x2401
	int 0x15

	jc A20GateError
	jmp A20GateSuccess

A20GateError:
	in al, 0x92
	or al, 0x02
	and al, 0xFE
	out 0x92, al

A20GateSuccess:
	cli
	lgdt[GDTR]
	mov eax, 0x4000003B
	mov cr0, eax

	jmp dword 0x18:(ProtectedMode - $$ + 0x10000)



[bits 32]
ProtectedMode:
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ss, ax
	mov esp, 0xFFFE
	mov ebp, 0xFFFE

	push (SwitchSuccessMessage -$$ + 0x10000)
	push 2
	push 0
	call PrintMessage
	add esp, 12
	
	jmp dword 0x18:0x10200

PrintMessage:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push eax
	push ecx
	push edx

	mov eax, dword[ebp+12]
	mov esi, 160
	mul esi
	mov edi, eax

	mov eax, dword[ebp+8]
	mov esi, 2
	mul esi
	add edi, eax

	mov esi, dword[ebp+16]

MessageLoop:
	mov cl, byte[esi]

	cmp cl, 0
	je MessageEnd

	mov byte[edi+0xB8000],cl

	add esi, 1
	add edi, 2

	jmp MessageLoop

MessageEnd:
	pop edx
	pop ecx
	pop eax
	pop edi
	pop esi
	pop ebp
	ret

align 8, db 0

dw 0x0000

GDTR:
	dw GDTEND - GDT - 1
	dd (GDT-$$+0x10000)

GDT:
	NULLDescriptor:
		dw 0x0000
		dw 0x0000
		db 0x00
		db 0x00
		db 0x00
		db 0x00

	IA_32eCODEDescriptor:
		dw 0xFFFF			;Limit [15:0]
		dw 0x0000			;Base[15:0]
		db 0x00				;Base[23:16]
		db 0x9A				;P=1, DPL=0, Code Segment, Execute/Read
		db 0xAF				;G=1, D=0, L=1, Limit[19:16]
		db 0x00				;Base[31:24]

	IA_32eDATADescriptor:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x92
		db 0xAF
		db 0x00

	CODEDescriptor:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x9A
		db 0xCF
		db 0x00

	DATADescriptor:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x92
		db 0xCF
		db 0x00
GDTEND:

SwitchSuccessMessage:
	db "Switch To Protected Mode Success ", 0

times 512 - ($ - $$) db 0x00