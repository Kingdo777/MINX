;bootLoader
%include "include/stdvar.inc";引入常用的常量定义
org	positionOfLoaderInMem
jmp	LoaderStart
;数据区
dataSection:
	string	db `Hello,I am BootLoader!\n\rI am very happy`
	strlen	equ	$-string
LoaderStart:
	mov ax,cs
	mov ds,ax
	mov es,ax
	;打印字符串
	mov bp,string
	mov cx,strlen
	xor dx,dx
	mov bx,0004h
	mov ax,1300h
	int 10h
	hlt
LoaderEnd:
