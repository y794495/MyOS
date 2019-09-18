[org 0x00]
[bits 16]

SECTION .text

jmp 0x7c0:start
TotalSectorCount: dw 2
Kernel32SectorCount: dw 2

start:
	mov ax, 0xB800
	mov es, ax
	mov ax, 0x7c0
	mov ds, ax
	
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFE
	mov bp, 0xFFFE
	
	mov si, 0
	
ScreenClearLoop:
	mov byte[es:si], 0
	
	mov byte[es:si+1],0x0A
	add si, 2
	cmp si, 80*25*2
	jl ScreenClearLoop
	
	push Message1
	push 0
	push 0
	call PrintMessage
	add sp, 6
	
	push IMageLoadingMessage
	push 1
	push 0
	call PrintMessage
	add sp, 6
	
ResetDisk:
	mov ax, 0
	mov dl, 0
	int 0x13
	jc HandleDiskError
	
	mov si, 0x1000
	mov es, si
	mov bx, 0x0000		;es:bx 주소로 디스크 내용 복사
	
	mov di, word[TotalSectorCount]
	
ReadData:
	cmp di,0
	je ReadEnd
	sub di, 0x1
	
	mov ah, 0x02
	mov al, 0x1
	mov ch, byte[TrackNumber]
	mov cl, byte[SectorNumber]
	mov dh, byte[HeadNumber]
	mov dl, 0x00
	int 0x13
	jc HandleDiskError
	
	add si, 0x0020		;다음섹터 (1020)
	
	mov es, si          ;다음에 저장될 주소
	
	mov al, byte[SectorNumber]
	add al, 0x01
	mov byte[SectorNumber], al
	cmp al, 19
	jl ReadData						;왼쪽값이 작으면 점프
		
	xor byte[HeadNumber], 0x01
	mov byte[SectorNumber], 0x01
	
	cmp byte[HeadNumber], 0x00
	
	jne ReadData
	
	add byte[TrackNumber], 0x01
	jmp ReadData
ReadEnd:

	push LoadingCompleteMessage
	push 1
	push 20
	call PrintMessage
	add sp, 6
	mov ax, 0x1000
	
	mov ds, ax
	mov es, ax
	cli

	jmp 0x1000:0x0000
	
	
HandleDiskError:
	push DiskErrorMessage
	push 1
	push 20
	call PrintMessage
	
	jmp $
	
PrintMessage:
	push bp
	mov bp, sp
	
	push es
	push si
	push di
	push ax
	push cx
	push dx
	
	mov ax, 0xB800
	
	mov es, ax
	mov ax, word[bp+6]
	mov si, 160
	mul si
	mov di, ax
	
	mov ax, word[bp+4]
	mov si, 2
	mul si
	add di, ax
	
	mov si, word[bp+8]
MessageLoop:
	mov cl, byte[si]
	
	
	cmp cl, 0
	je MessageEnd
	
	mov byte[es:di], cl
	
	add si, 1
	add di, 2
	
	
	jmp MessageLoop
	
MessageEnd:
	pop dx
	pop cx
	pop ax
	pop di
	pop si
	pop es
	pop bp
	ret
	
Message1:
	db "MyOS BootLoader Start", 0
	
DiskErrorMessage:
	db "Disk Error", 0
IMageLoadingMessage:
	db "OS Image Loading...", 0
LoadingCompleteMessage:
	db "Complete", 0
	
SectorNumber:
	db 0x02
HeadNumber:
	db 0x00
TrackNumber:
	db 0x00
	
times 510-($-$$) db 0x00

dw 0xAA55