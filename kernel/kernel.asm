selector_CORE_DATA_4G	equ	1*8
selector_CORE_CODE_4G	equ	2*8
SELECTOR_LDT			equ	5*8
SELECTOR_TSS			equ	6*8
;导入函数
extern	cstart
extern	puts
extern	exception_handler
extern	hardWareInt_handler
extern	breakPointDebug
extern	kernelMain
extern	clock_handler

extern	testFunc;测试函数
;导入全局变量
extern	gdt_ptr
extern	idt_ptr
extern	pcb_ptr
extern	tss
extern	Int_reEnter;时钟中断重入检测
extern	irq_table;硬件中断处理函数数组
extern	system_call_func_table;系统调用处理函数数组

section .data
	string  	db  `hello world!\nI am MINX\n`,0
	string1  	db  `SUCCESS!\n`,0
	string2  	db  `-`,0
	string3  	db  `*`,0
section .bss
	; StackSpace	times	2*1024	db	0	
	StackSpace	resb	2*1024;这个地方使用上面的那条语句将会产生一条警告（warning: attempt to initialize memory in BSS section `.bss': ignored），可能是因为.bss段是用来存放为初始化的全局变量的，而上面的语句则对数据进行了初始化操作
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

global	system_call

global	restart
global	test_in_asmA
global	test_in_asmB
_start:
	push	string
	push	04h
	call 	puts
	add		esp,8

	mov		esp,StackTop;重新切换堆栈
	sgdt	[gdt_ptr]
	call	cstart
	lgdt	[gdt_ptr]
	lidt	[idt_ptr]
	mov		ax,SELECTOR_LDT
	lldt	ax
	mov		ax,SELECTOR_TSS
	ltr		ax
	jmp		selector_CORE_CODE_4G:flush
flush:
	;sti
	;ud2
	;call 	testFunc
	call	kernelMain
	jmp		$

test_in_asmA:
	int 	20h
	jmp		$
test_in_asmB:
	int 	20h
	jmp		$
restart:
	mov		esp,[pcb_ptr];我extern过来的是pcb_ptr的地址，这一点很关键
	lea		eax,[esp+18*4];这两行代码包含居丰富的信息，我们把pcb的ss的下一成员的起始地址作为了tss中esp0，目的是在中断发生时涉及到了特权级的转化，此时将从tss中获取ss0，和esp0来进行堆栈的切换
	mov		[tss+4],eax;我们巧妙的将此时的esp0设为pcb中的特定位置，然后利用中断保护现场的操作对pcb中的数据进行赋值
re_enter:
	dec 	dword	[Int_reEnter]	
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp,4
	iretd

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
	hlt
	iretd


;  硬件中断
; ---------------------------------
%macro  hwint_master    1
	call	save
	;拒绝相同类型中断的重入
	in  	al,21h
	or 		al,(1<<%1)
	out 	21h,al
	;开启硬件中断
	mov		al,20h
	out		20h,al
	sti 	;开中断,保存现场结束后，打开中断以允许中断嵌套
;##############################################################
	push	%1
	call	[irq_table+4*%1]
	add		esp,4
;##############################################################
	;关中断，恢复现场
	cli	
	;打开该类型中断的重入
	in  	al,21h
	and		al,~(1<<%1)
	out 	21h,al
	ret
%endmacro

%macro  hwint_slave    1
	call	save
	;拒绝相同类型中断的重入
	in  	al,0A1h
	or 		al,(1<<(%1-8))
	out 	0A1h,al
	;开启硬件中断
	mov		al,20h
	out		20h,al
	sti 	;开中断,保存现场结束后，打开中断以允许中断嵌套
;##############################################################
	push	%1
	call	[irq_table+4*%1]
	add		esp,4
;##############################################################
	;关中断，恢复现场
	cli		
	;打开该类型中断的重入
	in  	al,0A1h
	and		al,~(1<<(%1-8))
	out 	0A1h,al
	ret
%endmacro

save:
	cld
	pushad
	push	ds
	push	es
	push	fs
	push	gs
	mov		si,ss
	mov		ds,si
	mov		es,si
	mov		es,si
	mov		fs,si
	mov		gs,si	
	mov		esi,esp
	inc 	dword	[Int_reEnter]
	cmp		dword	[Int_reEnter],0
	jne		re_enter_guid
	mov		esp,StackTop;切换到内核战
	push	restart
	jmp		[esi+4*12];eax是进程表的首地址，此操作是条转到call save后执行
re_enter_guid:
	push	re_enter
	jmp		[esi+4*12];eax是进程表的首地址，此操作是条转到call save后执行
	
hwint00:hwint_master	0; Interrupt routine for irq 0 (the clock).时钟中断
hwint01:hwint_master 	1; Interrupt routine for irq 1 (keyboard)
hwint02:hwint_master 	2; Interrupt routine for irq 2 (cascade!)
hwint03:hwint_master 	3; Interrupt routine for irq 3 (second serial)
hwint04:hwint_master 	4; Interrupt routine for irq 4 (first serial)
hwint05:hwint_master 	5; Interrupt routine for irq 5 (XT winchester)
hwint06:hwint_master 	6; Interrupt routine for irq 6 (floppy)
hwint07:hwint_master 	7; Interrupt routine for irq 7 (printer)
hwint08:hwint_slave 	8; Interrupt routine for irq 8 (realtime clock).
hwint09:hwint_slave 	9; Interrupt routine for irq 9 (irq 2 redirected)
hwint10:hwint_slave 	10; Interrupt routine for irq 10
hwint11:hwint_slave 	11; Interrupt routine for irq 11
hwint12:hwint_slave 	12; Interrupt routine for irq 12
hwint13:hwint_slave 	13; Interrupt routine for irq 13 (FPU exception)
hwint14:hwint_slave 	14; Interrupt routine for irq 14 (AT winchester)
hwint15:hwint_slave 	15; Interrupt routine for irq 15

;系统调用
system_call:
	call	save
	sti 	;开中断,保存现场结束后，打开中断以允许中断嵌套
	call	[system_call_func_table+4*eax]
	mov		[esi+11*4],eax;此处的作用是将系统调用的返回值eax写到进程表中
	cli		;关中断，恢复现场
	ret