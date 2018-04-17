;bootLoader
%include "include/stdvar.inc";引入常用的常量定义
%include "include/pm.inc";引入常用的常量定义
org	positionOfLoaderInMem
jmp	LoaderStart
;数据区
dataSection:
	%include 		"include/fat12_head_info.inc";引入Fat12的头部信息
	kernelFileName		db	'KERNEL  BIN'
	noKernelFileerror	db	'No Kernel!'
	string			db	`Welcome To PM!`,0
;--------------------gdt部分------------------------------------------------------------------------------------------
;					段地址		段界限		属性
	GDT_BEGIN:	Descriptor	0		,0		,0				;空描述符
	CORE_DATA_4G:	Descriptor	0		,0fffffh	,DA_DRW|DA_32|DA_LIMIT_4K	;内核数据段
	CORE_CODE_4G:	Descriptor	0		,0fffffh	,DA_CR|DA_32|DA_LIMIT_4K	;内核代码段
;gdt存储信息
	GDTLEN	equ	$-GDT_BEGIN
	GDTPTR	dw	GDTLEN-1
		dd	GDT_BEGIN	
;gdt选择子
	selector_CORE_DATA_4G	equ	CORE_DATA_4G-GDT_BEGIN
	selector_CORE_CODE_4G	equ	CORE_CODE_4G-GDT_BEGIN
;---------------------------------------------------------------------------------------------------------------------
LoaderStart:
	mov ax,cs
	mov ds,ax
	mov es,ax
;#############################################################################################
; 加载kernel.bin到内存	
%include	"include/loadKernel.asm"
;#############################################################################################
;进入保护模式并开启分页
	;加载gdt
	lgdt	[GDTPTR]
	;关中断	
	cli
	;打开线地址A20
	in 	al,92h
	or 	al,00000010b
	out	92h,al
	;打开控制寄存器CR0的PE位
	mov eax,cr0
	or eax,1
	mov cr0,eax
	;清空流水线，串行化执行代码
	jmp dword selector_CORE_CODE_4G:flush
bits	32
flush:;此处的物理地址：0x0000000000009131
	mov	ax,selector_CORE_DATA_4G
	mov	ds,ax
	mov	es,ax
	mov	gs,ax
	mov	fs,ax
	mov	ss,ax
	mov	esp,0x7c00

	mov 	ah,04h
	mov 	ebx,string
	call 	put_string		

	hlt
bits	16
;函数：给定逻辑扇区号，从软盘中读取一个扇区并加载到相应内存中
;参数：al-->逻辑扇区号  es:bx-->内存地址
%include	"lib/ReadSector.asm"
;函数：通过fat获取文件的下一簇号
;输入：ax-->当前簇号  es:bx要提供一个512字节的空间用来加载FAT
;输出：ax-->下一簇号
%include	"lib/GetNextClusByFat.asm"
;函数：关闭软驱马达
;无参数和返回值
%include	"lib/KillMotor.asm"
bits	32
;打印字符串的相关的一系列函数
;输入：ah->高亮;[ebx]->字符串地址（字符串必须以0结尾）
;输出：无
%include	"lib/put_string.asm"
LoaderEnd:
