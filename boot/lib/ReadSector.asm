;函数：给定逻辑扇区号，从软盘中读取一个扇区并加载到相应内存中
;参数：ax-->逻辑扇区号  es:bx-->内存地址
ReadSector:
	; -----------------------------------------------------------------------
	; 怎样由扇区号求扇区在磁盘中的位置 (扇区号 -> 柱面号, 起始扇区, 磁头号)
	; -----------------------------------------------------------------------
	; 设扇区号为 x
	;                          ┌ 柱面号 = y >> 1
	;       x           ┌ 商 y ┤
	; -------------- => ┤      └ 磁头号 = y & 1
	;  每磁道扇区数     │
	;                   └ 余 z => 起始扇区号 = z + 1
	push ax
	push bx
	push cx
	push dx
	div	byte [BPB_sectorPerTrack]
	ror	ax,8;让余数在al，商在ah
	inc al	
	mov cl,al;设置起始扇区号
	mov ch,ah
	shr	ch,1;设置柱面号
	mov dh,ah
	and dh,1;设置磁头号
	xor	dl,dl;设置驱动器号为A盘
	mov ax,0x0201;设置读取扇区数为1，设置命令号为02
	int 13h
	pop dx
	pop cx
	pop bx
	pop dx
	ret
