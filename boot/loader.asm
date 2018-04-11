;bootLoader
%include "include/stdvar.inc";引入常用的常量定义
org	positionOfLoaderInMem
jmp	LoaderStart
;数据区
dataSection:
	string	db `Hello,I am BootLoader!\nI am very happy`,0
LoaderStart:
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ebx,string
	call put_string
	hlt
LoaderEnd:
;打印字符串的相关的一系列函数
;输入：ah->高亮;[ds:ebx]->字符串地址（字符串必须以0结尾）
;输出：无
%include	"lib/put_string.asm"