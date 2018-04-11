;fat12文件系统的BPB（BIOS Program Block）结构，存放在boot sector最开始的部分（除了第一条跳转指令和nop空指令）
;--------------------------------------------
BS_OEMname				db	'ForrestY'	;厂商名，必须8字节

BPB_BytesPerSector		dw	512			;每扇区的字节数
BPB_SectorPerCluster	db	1			;没簇有多少扇区
BPB_SizeOfBootSector	dw	1			;Boot占用多少扇区
BPB_NumOfFats			db	2			;Fat表的数目
BPB_NumOfRootDirMaxFile	dw	224			;根目录的最大文件数
BPB_NumOfSector16		dw	2880		;扇区总数（全盘有两个面，每个面80个磁道，每个磁道18个扇区2*80*18=2880个扇区）
BPB_Media				db	0xf0		;媒体描述符
BPB_SectorPerFat		dw	9			;fat表所占扇区数
BPB_sectorPerTrack		dw	18			;磁道扇区数
BPB_NumOfHeads			dw	2			;磁头数（软盘的面数）
BPB_NumOfHideSector		dd	0			;隐藏扇区数
BPB_NumOfSector32		dd	0			;扇区总数（当BPB_NumOfSector16字段为0时用该字段的内容表示扇区的总数，该字段是32位的）

BS_DriveNum				db 	0			;13号中断的驱动器号
BS_Reserved1			db	0			;保留字段未使用
BS_BootSig				db	0x29		;扩展引导标记
BS_VolID				dd	0			;卷序列号
BS_VolLab				db	'XMLOVEYYTVM';卷标，必须11字节
BS_FileSysType			db	'FAT12   '	;文件系统类型，必须8个字节
;--------------------------------------------

;相关宏定义
;根目录所占的扇区数 =(根目录的最大文件数*目录项所占字节数+(扇区大小-1))/(扇区的大小)->(224*32+511)/512=14,其中+511是为了余数进位
SectorsOfRootDir		equ	14
;根目录的起始扇区号	=（BootSector所占扇区数+Fat所占扇区数*Fat数目）->1+9*2=19
startSectorOfRootDir	equ	19
;数据区开始扇区号=根目录的起始扇区号+根目录所占的扇区数=33，但是注意的是数据区的起始簇号是从2开始是的，因此在通过FAT项所提供的簇号转换为磁盘号时需要-2
startSectorOfFile		equ 31
