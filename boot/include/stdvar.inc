;我是全局头文件，里面都是一些常量或者宏的定义等等

positionOfLoaderInMem	equ	0x9000	;Loader加载到内存的位置（使用平坦模型）

;段描述符相关
;				G_D/B_L_AVL_offset_P_DPL_S_TYPE
RWG_data 	equ 1__1__0__0___0000__1_00__1_0010B 	;32位可读可写大粒度数据段
RWS_data 	equ 0__1__0__0___0000__1_00__1_0010B	;32位可读可写小粒度数据段
RWG_stack 	equ 1__1__0__0___0000__1_00__1_0110B	;32位可读可写大粒度栈段
RXS_code	equ 0__1__0__0___0000__1_00__1_1010B	;32位可读可执行小粒度非一致代码段

;FAT12目录项偏移
dirNameOffset		equ		0		;文件名
dirAttrOffset		equ		0xB		;文件属性
dirWriteTimeOffset	equ		0x16	;文件第一次写入时间
dirWriteDateOffset	equ		0x18	;文件第一次写入日期
dirFirstClusOffset	equ		0x1A	;文件起始簇号
dirSizeOffset		equ		0x1C	;文件大小

;显示样式
;--背景--				K R G B I R G B
flickering 			equ 1_0_0_0_0_0_0_0B;背景闪烁
back_black			equ 0_0_0_0_0_0_0_0B;黑色背景
back_blue			equ 0_0_0_1_0_0_0_0B;蓝色背景
back_green			equ 0_0_1_0_0_0_0_0B;绿色背景
back_cyan			equ 0_0_1_1_0_0_0_0B;青色背景
back_red			equ 0_1_0_0_0_0_0_0B;红色背景
back_magenta		equ 0_1_0_1_0_0_0_0B;品红色背景
back_brown			equ 0_1_1_0_0_0_0_0B;棕色背景
back_white			equ 0_1_1_1_0_0_0_0B;白色背景
;前景
lighting			equ 0_0_0_0_1_0_0_0B;前景加亮
front_black			equ 0_0_0_0_0_0_0_0B;黑色前景
front_blue			equ 0_0_0_0_0_0_0_1B;蓝色前景
front_green			equ 0_0_0_0_0_0_1_0B;绿色前景
front_cyan			equ 0_0_0_0_0_0_1_1B;青色前景
front_red			equ 0_0_0_0_0_1_0_0B;红色前景
front_magenta		equ 0_0_0_0_0_1_0_1B;品红色前景
front_brown			equ 0_0_0_0_0_1_1_0B;棕色前景
front_white			equ 0_0_0_0_0_1_1_1B;白色前景
