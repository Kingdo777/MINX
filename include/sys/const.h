#ifndef _MINX_CONST_H
#define _MINX_CONST_H
#include <stdint.h>

#define GDT_SIZE        128
#define LDT_SIZE        1
#define IDT_SIZE        256
#define TASK_STACK_SIZE 4 * 1024
#define NR_TASK         5
#define NR_USER_PROCESS 4
#define NR_MAX_USER_PROCESS 32//可容纳的最大用户进程数
#define NR_IRQ          16
#define NR_SYS_CALL     3
#define NR_TTY          2
#define NR_TTY_BUF      1024*4


#define HEX 16
#define DEC 10

/* VGA */
#define	CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	0xC	/* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	0xD	/* reg index of video mem start addr (LSB) */
#define	CURSOR_H	    0xE	/* reg index of cursor position (MSB) */
#define	CURSOR_L	    0xF	/* reg index of cursor position (LSB) */
#define	V_MEM_BASE	    0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	    0x8000	/* 32K: B8000H -> BFFFFH */

/* 8042 ports */
#define KB_DATA		0x60	/* I/O port for keyboard data
					Read : Read Output Buffer
					Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD		0x64	/* I/O port for keyboard command
					Read : Read Status Register
					Write: Write Input Buffer(8042 Command) */
#define LED_CODE	0xED
#define KB_ACK		0xFA


typedef void  (*hwint_handler)(int irq);//硬件中断处理函数数组
typedef int  (*system_call_var)();//硬件中断处理函数数组

//下面两个常量应用在printx系统调用上面，其数值并不是关键，其镶嵌在字符串的第一个字符上面，printx会检查要打印字符串的第一个字符，若是这两个标志，将会进行特殊处理
#define MAG_CH_PANIC 	1
#define MAG_CH_ASSERT 	2



/* Color */
/*
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */
#define	MAKE_COLOR(x,y)	((x<<4) | y) /* MAKE_COLOR(Background,Foreground) */

/* tasks */
/* 注意 TASK_XXX 的定义要与 global.c 中task[NR_TASK]数组对应 */
#define INVALID_DRIVER	-20
#define INTERRUPT	-10
#define TASK_TTY	0
#define TASK_SYS	1
#define TASK_HD		2
#define TASK_FS		3 
#define TASK_MM		4 
#define ANY		(NR_TASK + NR_USER_PROCESS + 10)
#define NO_TASK		(NR_TASK + NR_USER_PROCESS + 20)


/* ipc_type */
#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

/* Process */
#define SENDING   0x02	/* set when proc trying to send */
#define RECEIVING 0x04	/* set when proc trying to recv */
#define WAITING   0x08	/* set when proc waiting for the child to terminate */
#define HANGING   0x10	/* set when proc exits without being waited by parent */
#define FREE_SLOT 0x20	/* set when proc table entry is not used
			 * (ok to allocated to a new process)

/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS, GET_PID, GET_RTC_TIME,

	/* FS */
	OPEN, CLOSE, READ, WRITE, LSEEK, STAT, UNLINK,

	/* FS & TTY */
	SUSPEND_PROC, RESUME_PROC,

	/* MM */
	EXEC, WAIT,

	/* FS & MM */
	FORK, EXIT,

	/* TTY, SYS, FS, MM, etc */
	SYSCALL_RET,

	/* message type for drivers */
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};

/* macros for messages */
#define	FD		u.m3.m3i1
#define	PATHNAME	u.m3.m3p1
#define	FLAGS		u.m3.m3i1
#define	NAME_LEN	u.m3.m3i2
#define	BUF_LEN		u.m3.m3i3
#define	CNT		u.m3.m3i2
#define	REQUEST		u.m3.m3i2
#define	PROC_NR		u.m3.m3i3
#define	DEVICE		u.m3.m3i4
#define	POSITION	u.m3.m3l1
#define	BUF		u.m3.m3p2
#define	OFFSET		u.m3.m3i2
#define	WHENCE		u.m3.m3i3

#define	PID		u.m3.m3i2
#define	RETVAL		u.m3.m3i1
#define	STATUS		u.m3.m3i1





#define	DIOCTL_GET_GEO	1

/* Hard Drive */
#define SECTOR_SIZE		512
#define SECTOR_BITS		(SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT	9


#define	MAX_DRIVES		2		//最大硬盘数目
#define	NR_PART_PER_DRIVE	4	//每块硬盘的主分区和扩展分区最大数目
#define	NR_SUB_PER_PART		16	//每块扩展分区的逻辑分区最大数目
#define	NR_SUB_PER_DRIVE	(NR_SUB_PER_PART * NR_PART_PER_DRIVE)//每块硬盘的最大逻辑分区数目
#define	NR_PRIM_PER_DRIVE	(NR_PART_PER_DRIVE + 1)//每块硬盘拥有用的主分区数目，此处的+1是指硬盘本身

/**
 * @def MAX_PRIM_DEV
 * Defines the max minor number of the primary partitions.
 * If there are 2 disks, prim_dev ranges in hd[0-9], this macro will
 * equals 9.
 */
#define	MAX_PRIM		(MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)//主分区最大值，因为有两块硬盘，因此最大值为9也就是说0-9是主分区，大于9的都是逻辑分区

#define	MAX_SUBPARTITIONS	(NR_SUB_PER_DRIVE * MAX_DRIVES)//逻辑分区的最大值

#define	P_PRIMARY	0
#define	P_EXTENDED	1

/**
* major device numbers (corresponding to kernel/global.c::dd_map[]) 
*定义主设备号，此处要与dd_map对应，因为不同的主设备要交给对应的设备驱动程序
*/
#define	NO_DEV			0
#define	DEV_FLOPPY		1
#define	DEV_CDROM		2
#define	DEV_HD			3
#define	DEV_CHAR_TTY	4
#define	DEV_SCSI		5
/** make device number from major and minor numbers 
*	MAKE_DEV用于生成设备号，其中a是主设备号，b是次设备号
*/
#define	MAJOR_SHIFT		8
#define	MAKE_DEV(a,b)		((a << MAJOR_SHIFT) | b)
/** separate major and minor numbers from device number 
*	通过设备号获取主设备号和次设备号
*/
#define	MAJOR(x)		((x >> MAJOR_SHIFT) & 0xFF)
#define	MINOR(x)		(x & 0xFF)

/* device numbers of hard disk */
#define	MINOR_hd1a		0x10
#define	MINOR_hd2a		0x20
#define	MINOR_hd2b		0x21
#define	MINOR_hd3a		0x30
#define	MINOR_hd4a		0x40


#define	ROOT_DEV		MAKE_DEV(DEV_HD, MINOR_BOOT)

#define	INVALID_INODE		0
#define	ROOT_INODE		1


#define ORANGES_PART	0x99	/* Orange'S partition */
#define NO_PART		0x00	/* unused entry */
#define EXT_PART	0x05	/* extended partition */

#define	NR_FILES	64
#define	NR_FILE_DESC	64	/* FIXME */
#define	NR_INODE	64	/* FIXME */
#define	NR_SUPER_BLOCK	8


/* INODE::i_mode (octal, lower 32 bits reserved) */
#define I_TYPE_MASK     0170000
#define I_REGULAR       0100000
#define I_BLOCK_SPECIAL 0060000
#define I_DIRECTORY     0040000
#define I_CHAR_SPECIAL  0020000
#define I_NAMED_PIPE	0010000

#define	is_special(m)	((((m) & I_TYPE_MASK) == I_BLOCK_SPECIAL) ||	\
			 (((m) & I_TYPE_MASK) == I_CHAR_SPECIAL))

#define	NR_DEFAULT_FILE_SECTS	2048 /* 2048 * 512 = 1MB */

#endif