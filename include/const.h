#ifndef _MINX_CONST_H
#define _MINX_CONST_H
#include <stdint.h>

#define GDT_SIZE        128
#define LDT_SIZE        1
#define IDT_SIZE        256
#define TASK_STACK_SIZE 4 * 1024
#define NR_TASK         1
#define NR_USER_PROCESS 3
#define NR_IRQ          16
#define NR_SYS_CALL     1
#define NR_TTY          4
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
typedef void  (*system_call_var)();//硬件中断处理函数数组

#endif