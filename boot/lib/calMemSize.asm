;宏,计算内存的大小，根据经验，ARDS中可用的内存有两段，一段是从0到9ffff，另一段是从100000到最大
;注意，此宏应用于保护模式下
%macro calMemSize 7;7个参数,ARDS_BUF（%1）、ARDStruct（%2）、ARDS_BaseAddrLow（%3）、ARDS_LengthLow（%4）、ARDS_Type（%5）、MemSize（%6）、ARDS_NUMBER（%7）
	mov		esi,%1
	mov		edi,%2
	mov		ecx,[%7]
calMemSize_loop:
	push	ecx
	mov		ecx,20
	movsb
	cmp		dword	[%5],1
	jne		calMemSize_next
	mov		eax,[%3]
	add		eax,dword	[%4]
	mov		[%6],eax
calMemSize_next:
	add		esi,20
	pop		ecx
	loop	calMemSize_loop
%endmacro
