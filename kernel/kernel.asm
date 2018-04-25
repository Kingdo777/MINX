;导入函数
extern	resetGdt
extern	puts
;导入全局变量
extern	gdt_ptr;是一个大小为6的字节数组

section .data
	string  	db  `hello world!\nI am MINX\n`,0
	string1  	db  `END!`,0
	StackSpace	times	2*1024	db	0	
	StackTop:	;栈顶指针

section .text
global  _start
_start:
	push	string
	push	04h
	call 	puts

	mov		esp,StackTop;重新切换堆栈
	sgdt	[gdt_ptr]
	call	resetGdt
	lgdt	[gdt_ptr]

	hlt
	hlt