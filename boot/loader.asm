;bootLoader
%include "include/stdvar.inc";引入常用的常量定义
%include "include/pm.inc";引入保护模式的常用宏以及常量
%include "lib/getMemARDS.asm";宏函数
%include "lib/calMemSize.asm";宏函数
org	positionOfLoaderInMem
jmp	LoaderStart
;数据区
dataSection:
	%include 		"include/fat12_head_info.inc";引入Fat12的头部信息
	kernelFileName		db	'KERNEL  BIN'
	noKernelFileerror	db	'No Kernel!'
;--------------------MEM信息buf---------------------------------------------------------------------------------------
ARDS_BUF times	400		db 0;每个ARDS大小为20字节，这里预留20ARDS的空间
ARDS_NUMBER				dd 0;
getARDS_errorInfo		db "get ARDS wrong"
errorInfoLen			equ	$-getARDS_errorInfo
MemSize					dd	0
ARDStruct:;ARDS对应的结构体
	ARDS_BaseAddrLow:		dd	0
	ARDS_BaseAddrHigh:		dd	0
	ARDS_LengthLow:			dd	0
	ARDS_LengthHigh:		dd	0
	ARDS_Type:				dd	0
;--------------------栈空间-------------------------------------------------------------------------------------------
BaseOfStack:
times 	1024	db	0
TopOfStack:
;--------------------gdt部分------------------------------------------------------------------------------------------
;					段地址		段界限		属性
	GDT_BEGIN:	Descriptor		0		,0		,0				;空描述符
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
;获取内存信息，因为需要用到int 15h中断，因此要在进入保护模式之前调用;物理地址0x00000000000096b5
getMemARDS	ARDS_NUMBER,ARDS_BUF,getARDS_errorInfo,errorInfoLen
;计算内存大小，并根据内存大小进行分页
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

;-------------------32位保护模式代码-------------------------------------------------------
section .code_32
bits	32
flush:;此处的物理地址：
	mov	ax,selector_CORE_DATA_4G
	mov	ds,ax
	mov	es,ax
	mov	gs,ax
	mov	fs,ax
	mov	ss,ax
	mov	esp,TopOfStack
;根据ARDS获取可用内存的大小
calMemSize  ARDS_BUF,ARDStruct,ARDS_BaseAddrLow,ARDS_LengthLow,ARDS_Type,MemSize,ARDS_NUMBER
;下面开始设置页表和页目录，并开启分页
	xor		edx,edx
	mov 	eax,[MemSize]
	mov		ebx,4*1024*1024
	div		ebx
	test	edx,edx
	jz		@1
	inc		eax;余数不为0,则多创建一个页表
@1:
	push	eax;
	mov		ecx,eax
	mov		edi,positionOfPDTFileInMem
	mov		eax,positionOfPGTFileInMem|PG_P|PG_RWW|PG_USS
@2:;下面三行代码是对PDT的初始化
	stosd
	add		eax,4*1024
	loop	@2
	;下面开始初始化页表
	pop		eax
	mov		ebx,1024
	mul		ebx;计算页表项的数目，已知页表项最多也就1024×1024，所以不会超出32位寄存器
	mov		ecx,eax
	mov		edi,positionOfPGTFileInMem
	xor		eax,eax
	mov		eax,PG_P|PG_RWW|PG_USS
@3:;
	stosd
	add		eax,4*1024
	loop	@3	
;PGT、PDT初始化完毕，下面开启分页
	mov		eax,positionOfPDTFileInMem
	mov		cr3,eax
	mov		eax,cr0
	or		eax,80000000h
	mov		cr0,eax
	jmp		pg_flush
pg_flush:
;######################################################################################################
;根据elf head以及programe head的信息，将kernel相应的段进行重新安排
	mov		cx,[positionOfKernelFileInMem+2ch];获取programe head个数
	movzx	ecx,cx
	mov		ebx,[positionOfKernelFileInMem+1ch];获取programe head偏移
	add		ebx,positionOfKernelFileInMem;获取programe head地址
@4:
	push	ecx
	mov		eax,[ebx]
	cmp		eax,0
	jz		@5
	mov		ecx,[ebx+10h];段大小
	mov		esi,[ebx+04h];在文件中的偏移
	add		esi,positionOfKernelFileInMem
	mov		edi,[ebx+08h];在内存中的偏移
	rep		movsb
@5:
	add		ebx,20h
	pop		ecx
	loop	@4
;######################################################################################################
;转交控制权，进入操作系统
jmp	entryAddrOfKernel
;打印字符串的相关的一系列函数
;输入：ah->高亮;[ebx]->字符串地址（字符串必须以0结尾）
;输出：无
%include	"lib/put_string.asm"
LoaderEnd:
