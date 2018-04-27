selector_CORE_DATA_4G	equ	08h
selector_CORE_CODE_4G	equ	10h
;导入函数
extern	cstart
extern	puts
extern	exception_handler
extern	hardWareInt_handler
extern	breakPointDebug
;导入全局变量
extern	gdt_ptr
extern	idt_ptr

section .data
	string  	db  `hello world!\nI am MINX\n`,0
	string1  	db  `SUCCESS!\n`,0
	StackSpace	times	2*1024	db	0	
	StackTop:	;栈顶指针

section .text
global  _start

global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15

_start:
	push	string
	push	04h
	call 	puts

	mov		esp,StackTop;重新切换堆栈
	sgdt	[gdt_ptr]
	call	cstart
	lgdt	[gdt_ptr]
	lidt	[idt_ptr]
	jmp		selector_CORE_CODE_4G:flush
flush:
	sti
	;ud2
	;int 	0x33

	push	string1
	push	03h
	call 	puts

	hlt
	hlt
	hlt
	hlt
	hlt
	hlt


;  异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:;断点调试，我们让他打印栈的信息
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	iret


;  硬件中断
; ---------------------------------
%macro  hwint    1
    push    %1
    call    hardWareInt_handler
    add     esp, 4
    iret
%endmacro
; ---------------------------------


hwint00:hwint    0; Interrupt routine for irq 0 (the clock).
hwint01:hwint    1; Interrupt routine for irq 1 (keyboard)
hwint02:hwint    2; Interrupt routine for irq 2 (cascade!)
hwint03:hwint    3; Interrupt routine for irq 3 (second serial)
hwint04:hwint    4; Interrupt routine for irq 4 (first serial)
hwint05:hwint    5; Interrupt routine for irq 5 (XT winchester)
hwint06:hwint    6; Interrupt routine for irq 6 (floppy)
hwint07:hwint    7; Interrupt routine for irq 7 (printer)

; ---------------------------------
%macro  hwint_slave     1
    push    %1
    call    hardWareInt_handler
    add     esp, 4
    iret
%endmacro
; ---------------------------------


hwint08:hwint    ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8


hwint09:hwint    ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9


hwint10:hwint    ; Interrupt routine for irq 10
        hwint_slave     10


hwint11:hwint    ; Interrupt routine for irq 11
        hwint_slave     11


hwint12:hwint    ; Interrupt routine for irq 12
        hwint_slave     12


hwint13:hwint    ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13


hwint14:hwint    ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14


hwint15:hwint    ; Interrupt routine for irq 15
        hwint_slave     15