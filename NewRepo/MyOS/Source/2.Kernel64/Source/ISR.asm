[bits 64]

SECTION .text
;RCX > RDX > R8 > R9 (À©µµ¿ì)
extern CommonExceptionHandler, CommonInterruptHandler, KeyboardHandler, TimerHandler

global ISRDivideError, ISRDebug, ISRNMI, ISRBreakPoint, ISROverflow
global ISRBoundRangeExceeded, ISRInvalidOpcode, ISRDeviceNotAvailable, ISRDoubleFault
global ISRCoprocessorSegmentOverrun, ISRInvalidTSS, ISRSegmentNotPresent
global ISRStackSegmentFault, ISRGeneralProtection, ISRPageFault, ISR15
global ISRFPUError, ISRAlignmentCheck, ISRMachineCheck, ISRSIMDError, ISRETCException

global ISRTimer, ISRKeyboard, ISRSlavePIC, ISRSerial2, ISRSerial1, ISRParallel2
global ISRFloppy, ISRParallel1, ISRRTC, ISRReserved, ISRNotUsed1, ISRNotUsed2
global ISRMouse, ISRCoprocessor, ISRHDD1, ISRHDD2, ISRETCInterrupt

%macro ExceptionSAVECONTEXT 0
	push rbp
	mov rbp, rsp
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
    sub rsp, 0x20
%endmacro

%macro SAVECONTEXT 0
	push rbp
	mov rbp, rsp
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
    sub rsp, 0x20
%endmacro


%macro LOADCONTEXT 0
    add rsp, 0x20
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

ISRDivideError:
	ExceptionSAVECONTEXT
	
	mov rcx, 0
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRDebug:
	ExceptionSAVECONTEXT
	
	mov rcx, 1
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRNMI:
	ExceptionSAVECONTEXT
	
	mov rcx, 2
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRBreakPoint:
	ExceptionSAVECONTEXT
	
	mov rcx,3
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISROverflow:
	ExceptionSAVECONTEXT
	
	mov rcx, 4
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRBoundRangeExceeded:
	ExceptionSAVECONTEXT
	
	mov rcx, 5
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRInvalidOpcode:
	ExceptionSAVECONTEXT
	
	mov rcx, 6
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRDeviceNotAvailable:
	ExceptionSAVECONTEXT
	
	mov rcx, 7
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRDoubleFault:
	ExceptionSAVECONTEXT
	
	mov rcx, 8
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp,8
	iretq
	
ISRCoprocessorSegmentOverrun:
	ExceptionSAVECONTEXT
	
	mov rcx, 9
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq

ISRInvalidTSS:
	ExceptionSAVECONTEXT
	
	mov rcx, 10
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp, 8
	iretq
	
ISRSegmentNotPresent:
	ExceptionSAVECONTEXT
	
	mov rcx, 11
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp, 8
	iretq

ISRStackSegmentFault:
	ExceptionSAVECONTEXT
	
	mov rcx, 12
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp,8
	iretq

ISRGeneralProtection:
	ExceptionSAVECONTEXT
	
	mov rcx, 13
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp, 8
	iretq
	
ISRPageFault:
	ExceptionSAVECONTEXT
	
	mov rcx, 14
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp, 8
	iretq
	
ISR15:
	ExceptionSAVECONTEXT
	
	mov rcx, 15
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRFPUError:
	ExceptionSAVECONTEXT
	
	mov rcx, 16
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRAlignmentCheck:
	ExceptionSAVECONTEXT
	
	mov rcx, 17
	mov rdx, qword[rbp+8]
	call CommonExceptionHandler
	
	LOADCONTEXT
	add rsp, 8
	iretq
	
ISRMachineCheck:
	ExceptionSAVECONTEXT
	
	mov rcx, 18
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq

ISRSIMDError:
	ExceptionSAVECONTEXT
	
	mov rcx, 19
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRETCException:
	ExceptionSAVECONTEXT
	
	mov rcx, 20
	call CommonExceptionHandler
	
	LOADCONTEXT
	iretq
	
ISRTimer:
	SAVECONTEXT
	
	mov rcx, 32
   	call TimerHandler

	LOADCONTEXT    
	iretq

ISRKeyboard:
	SAVECONTEXT
	
	mov rcx, 33
	call KeyboardHandler
	
	LOADCONTEXT
	iretq

ISRSlavePIC:
	SAVECONTEXT
	
	mov rcx, 34
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq

ISRSerial2:
	SAVECONTEXT
	
	mov rcx, 35
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRSerial1:
	SAVECONTEXT
	
	mov rcx, 36
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRParallel2:
	SAVECONTEXT
	
	mov rcx, 37
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRFloppy:
	SAVECONTEXT
	
	mov rcx, 38
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq

ISRParallel1:
	SAVECONTEXT
	
	mov rcx, 39
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRRTC:
	SAVECONTEXT
	
	mov rcx, 40
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRReserved:
	SAVECONTEXT
	
	mov rcx, 41
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRNotUsed1:
	SAVECONTEXT
	
	mov rcx, 42
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRNotUsed2:
	SAVECONTEXT
	
	mov rcx, 43
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRMouse:
	SAVECONTEXT
	
	mov rcx, 44
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRCoprocessor:
	SAVECONTEXT
	
	mov rcx, 45
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRHDD1:
	SAVECONTEXT
	
	mov rcx, 46
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRHDD2:
	SAVECONTEXT
	
	mov rcx, 47
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq
	
ISRETCInterrupt:
	SAVECONTEXT
	
	mov rcx, 48
	call CommonInterruptHandler
	
	LOADCONTEXT
	iretq

Message db "KILL",0