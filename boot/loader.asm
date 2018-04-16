;bootLoader
%include "include/stdvar.inc";引入常用的常量定义
org	positionOfLoaderInMem
jmp	LoaderStart
;数据区
dataSection:
	%include 		"include/fat12_head_info.inc";引入Fat12的头部信息
	kernelFileName		db	'KERNEL  BIN'
	noKernelFileerror	db	'No Kernel!'
LoaderStart:
	mov ax,cs
	mov ds,ax
	mov es,ax
	
	;复位软盘
	xor ah,ah
	xor dl,dl
	int 0x13
	;从软盘中寻找kernel.bin
	mov bx,positionOfKernelFileInMem
	mov ax,startSectorOfRootDir
	mov ecx,SectorsOfRootDir
loopInRootDir:
	push ax
	call ReadSector
	push ecx
	mov ecx,16
	mov si,kernelFileName
	mov di,positionOfKernelFileInMem
loopInSector:;在扇区中循环，扇区512字节，16个目录项
	push ecx
	mov ecx,11
	repe cmpsb
	jz findKernelFile
	mov si,kernelFileName
	and di,0xffe0
	add di,32
	pop ecx
	loop loopInSector
	xor ax,ax
	mov es,ax
	mov bx,positionOfKernelFileInMem
	pop ecx
	pop ax
	inc ax
	loop loopInRootDir
	;打印错误信息，为了节省空间，直接调用13号中断
	;清屏(为了方便直接切换活动页)
	mov ax,0501h
    	int 0x10
	;打印字符串
	xor ax,ax
	mov es,ax
	mov bp,noKernelFileerror
	mov cx,10
	xor dx,dx
	mov bh,01h
	mov bl,04h
	mov ah,0x13
	int 10h
	hlt
findKernelFile:;找到了kernel.bin所在的项
	and di,0xffe0
	add di,dirFirstClusOffset
	mov ax,[es:di];获取文件的起始簇号
	push ax;备份一下
	xor bx,bx
	mov es,bx
	mov bx,positionOfKernelFileInMem
LoadeKernelFile:;加载Loader到内存
	add ax,startSectorOfFile;转化为扇区号
	call ReadSector
	pop ax
	add bx,word [BPB_BytesPerSector]
	call GetNextClusByFat
	cmp ax,0xff8
	jae LoadeKernelFileEnd
	push ax
	jmp LoadeKernelFile
LoadeKernelFileEnd:;加载结束
	
	hlt
;函数：给定逻辑扇区号，从软盘中读取一个扇区并加载到相应内存中
;参数：al-->逻辑扇区号  es:bx-->内存地址
%include	"lib/ReadSector.asm"
;函数：通过fat获取文件的下一簇号
;输入：ax-->当前簇号  es:bx要提供一个512字节的空间用来加载FAT
;输出：ax-->下一簇号
%include	"lib/GetNextClusByFat.asm"

LoaderEnd:
