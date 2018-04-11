;bootSector的任务只有一个，那就是将bootLoader加载到内存，并将控制权转交给loader
%include "include/stdvar.inc";引入常用的常量定义
org 0x7c00

jmp BOOT_START
nop;此指令必不可少
%include "include/fat12_head_info.inc";引入Fat12的头部信息
loaderName: db 	'loader  bin';中间是空格，因为fat12文件系统仅仅支持8字节文件名和3字节扩展名
errorInfo:	db	'no Loader',0

BOOT_START:
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp,0x7c00
	
	;复位软盘
	xor ah,ah
	xor	dl,dl
	int 0x13
	;从软盘中寻找loade.bin
	mov bx,positionOfLoaderInMem
	mov ax,startSectorOfRootDir
	push ax
	mov ecx,SectorsOfRootDir
loopInRootDir:
	call ReadSector
	push ecx
	mov ecx,16
	mov si,loaderName
	xor di,di
	push ecx
loopInSector:;在扇区中循环，扇区512字节，16个目录项
	mov ecx,11
	repe cmpsb
	jz findLoader
	mov si,loaderName
	and di,00100000b
	add di,32
	pop ecx
	loop loopInSector
	mov ax,positionOfLoaderInMem
	mov es,ax
	xor bx,bx
	pop ecx
	pop ax
	inc ax
	loop loopInRootDir
	mov ah,back_black+front_red
	mov ax,errorInfo
	call put_string
	hlt
findLoader:;找到了loader.bin所在的项
	and di,00100000b
	add di,dirFirstClusOffset
	mov ax,[es:di];获取文件的起始簇号
	push ax;备份一下
	mov bx,positionOfLoaderInMem
	mov es,bx
	xor bx,bx
LoadeLoader:;加载Loader到内存
	add ax,startSectorOfFile;转化为扇区号
	call ReadSector
	pop ax
	add bx,word [BPB_BytesPerSector]
	call GetNextClusByFat
	cmp ax,0xff8
	jae	LoadeLoaderEnd
	push ax
	jmp LoadeLoader
LoadeLoaderEnd:;加载结束
;将控制权转交给loader
	jmp positionOfLoaderInMem
;函数：给定逻辑扇区号，从软盘中读取一个扇区并加载到相应内存中
;参数：al-->逻辑扇区号  es:bx-->内存地址
%include	"lib/ReadSector.asm"
;打印字符串的相关的一系列函数
;输入：ah->高亮;[ds:ebx]->字符串地址（字符串必须以0结尾）
;输出：无
%include	"lib/put_string.asm"
;函数：通过fat获取文件的下一簇号
;输入：ax-->当前簇号  es:bx要提供一个512字节的空间用来加载FAT
;输出：ax-->下一簇号
%include	"lib/GetNextClusByFat.asm"
BOOT_END:dw 0xaa55