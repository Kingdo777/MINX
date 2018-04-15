;函数：通过fat获取文件的下一簇号
;输入：ax-->当前簇号  es:bx要提供一个512字节的空间用来加载FAT
;输出：ax-->下一簇号
GetNextClusByFat:
;首先确定数据在那个磁盘（按第一个磁盘算，因为可能跨磁盘），磁盘数共有9块
;簇号即项号乘3除2后表示起始的字节，除以512，商加1（第一块是BootSector）即磁盘号
;起始字节加1后若能被512整除，表示需要跨磁盘，将改字节加载到ah，下一磁盘块的0号字节加载到al，否则不需要跨磁盘，直接加载一个字到ax
;簇号若为偶数，去改字节为高位，即将ax右移4位即可，若为奇数取后半字节为高位，即与0x0fff与运算
push ebx
push ecx
push edx
push ax;保存簇号
mov cx,3
mul cx
mov cx,2
div cx;此时ax中存放的是起始字节号
push ax
mov cx,512
xor dx,dx
div cx
inc ax;此时ax中存放的是起始磁盘号
call ReadSector
pop cx;恢复起始字节号
inc cx
test cx,0x01ff
dec cx
jnz	InOneSector;在同一扇区
add bx,cx
mov dh,[es:bx]
sub	bx,cx
inc ax
call ReadSector
mov al,[es:bx]
mov ah,dh
jmp calNextClus
InOneSector:
add bx,cx
mov ax,[es:bx]
sub	bx,cx
calNextClus:
pop dx;恢复簇号
test dx,1
jnz isOddNum;是奇数
shr ax,4
jmp GetNextClusByFat_END;运算结束
isOddNum:
and ax,0x0fff
GetNextClusByFat_END:
pop edx
pop ecx
pop ebx
ret
