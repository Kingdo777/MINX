;宏，获取内存ARDS，参数3个，结构数目(%1)、用于存放结构信息的缓冲区地址(%2)、错误信息(%3)、错误信息长度(%4)
%macro getMemARDS 3
	mov	ebx,0
	mov	di,%2
.loop:
	mov	eax,0E820h
	mov	ecx,20
	mov	edx,0534D4150h
	int	15h
	jc	getARDS_wrong
	add	di,20
	inc	word	[%1]
	cmp	ebx,0
	jne	.loop
	jmp	getARDS_success
getARDS_wrong:
	mov	word [%1],0
	;清屏(为了方便直接切换活动页)
	mov ax,0501h
    	int 0x10
	;打印字符串
	xor ax,ax
	mov es,ax
	mov bp,%3
	mov cx,%4
	xor dx,dx
	mov bh,01h
	mov bl,04h
	mov ah,0x13
	int 10h
	hlt
getARDS_success:
%endmacro ; 
