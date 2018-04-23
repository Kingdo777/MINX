section .data
	string  db  `hello world!\nI am MINX!`,0
section .text
global  _start
_start:
	mov 	ah,04h
	mov 	ebx,string
	call 	put_string		
	hlt

;------------------------------func------------------------------------------------------
;打印字符串的相关的一系列函数
;输入：ah->高亮;[ebx]->字符串地址（字符串必须以0结尾）
;输出：无
%include	"lib/put_string.asm"
