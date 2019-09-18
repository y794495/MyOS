[bits 64]

SECTION .text

global InPortByte, OutPortByte, LoadGDTR, LoadTR, LoadIDTR
global EnableInterrupt, DisableInterrupt, ReadRFLAGS
global ReadTSC
global SwitchContext

;RDI > RSI > RDX > RCX > R8 > R9 순서 파라미터(정수형) (리눅스)
;RCX > RDX > R8 > R9 (윈도우)

InPortByte:			;파라미터 : 포트번호
	push rdx        ;함수에서 임시로 사용하는 레지스터를 스택에 저장
					;함수의 마지막 부분에서 스택에 삽입된 값을 꺼내 복원

	mov rdx, rcx    ;RDX 레지스터에 파라미터 1(포트번호)를 저장
	mov rax, 0      ;RAX 레지스터를 초기화
	in al, dx       ;DX 레지스터에 저장된 포트 어드레스에서 한 바이트를 읽어
                    ;AL 레지스터에 저장, AL 레지스터는 함수의 반환 값으로 사용됨

	pop rdx         ;함수에서 사용이 끝난 레지스터를 복원
	ret             ;함수를 호출한 다음 코드의 위치로 복귀
	                ;RAX값을 반환


OutPortByte:		;파라미터 : 포트번호(RCX), 데이터(RDX)
	push rax        ;함수의 마지막 부분에서 스택에 삽입된 값을 꺼내 복원

    mov rax, rdx    ;RAX 레지스터에 파라미터 2(데이터)를 저장
	mov rdx, rcx    ;RDX 레지스터에 파라미터 1(포트번호)를 저장
	
	out dx, al      ;DX 레지스터에 저장된 포트 어드레스에 AL 레지스터에 저장된 한 바이트를 씀

	pop rax         ;함수에서 사용이 끝난 레지스터를 복원
	ret             ;함수를 호출한 다음 코드의 위치로 복귀
                    ;반환 없음

LoadGDTR:
    lgdt[rcx]
    ret

LoadTR:
    ltr cx
    ret

LoadIDTR:
    lidt[rcx]
    ret

EnableInterrupt:
    sti
    ret

DisableInterrupt:
    cli
    ret

ReadRFLAGS:
    pushfq
    pop rax

    ret

;타임스탬프 카운터를 읽어서 반환
ReadTSC:
    push rdx

    rdtsc

    shl rdx, 32
    or rax, rdx

    pop rdx
    ret


%macro SaveContext 0
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov ax, ds
    push rax
    mov ax, es
    push rax
    push fs
    push gs
%endmacro

%macro LoadContextMacro 0
    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro 

;현재 콘텍스트를 저장하고 next Task에서 콘텍스트를 복구
;CurrentContext, Next Context
SwitchContext:
    push rbp                ;스택에 RBP 레지스터를 저장하고 RSP 레지스터를 RBP에 저장
    mov rbp, rsp

;Current Context가 NULL이면 콘텍스트를 저장할 필요 없음
    pushfq                  ;아래의 cmp의 결과로 RFLAGS 레지스터가 변하지 않도록 스택에 저장
    cmp rcx, 0              ;CurrentContext가 NULL이면 콘텍스트 복원으로 바로 이동
    je LoadContext
    popfq                   ;스택에 저장한 RFLAGS 레지스터를 복원

;현재 태스크의 콘텍스트를 저장
    push rax                ;콘텍스트 영역의 오프셋으로 사용할 RAX 레지스터를 스택에 저장

    ;SS, RSP, RFLAGS, CS, RIP 레지스터 순서대로 삽입
    mov ax,ss                   ;SS 레지스터 저장
    mov qword[rcx+(23 * 8)],rax

    mov rax,rbp
    add rax,16
    mov qword[rcx+(22 * 8)],rax

    pushfq
    pop rax
    mov qword[rcx+(21 * 8)],rax

    mov ax,cs
    mov qword[rcx+(20 * 8)],rax

    mov rax,qword[rbp + 8]        ;RIP 레지스터를 Return Address로 설정하여 rbp+8 = 복귀주소
    mov qword[rcx+(19 * 8)],rax   ;다음 콘텍스트 복원 시에 이 함수를 호출한 위치로 이동하게 함
    
    ;저장한 레지스터를 복구한 후 인터럽트가 발생했을 때처럼 나머지 콘텍스트를 모두 저장
    pop rax
    pop rbp

    ;가장 끝부분에 SS, RSP, RFLAGS, CS, RIP 레지스터를 저장했으므로, 이전 영역에
    ;push 명령어로 콘텍스트를 저장하기 위해 스택을 변경
    add rcx, (19 * 8)
    mov rsp,rcx                   ;RSP 레지스터를 현재 콘텍스트 자료구조의 19번째로 저장
    sub rcx, (19 * 8)

    SaveContext                   ;이후 push 명령으로만 콘텍스트 자료구조에 레지스터들을 저장 가능

LoadContext:
    mov rsp, rdx                  ;next 콘텍스트의 콘텍스트로 교체

    LoadContextMacro              ;pop하여 교체
    iretq                         ;rbp 까지 다 pop한 상태면 그 다음 19 offset은 rip이기때문에 iretq로 rip를 복귀주소로 활용할수 있음