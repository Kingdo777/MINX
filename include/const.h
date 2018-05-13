#ifndef _MINX_CONST_H
#define _MINX_CONST_H
#include <stdint.h>

#define GDT_SIZE        128
#define LDT_SIZE        1
#define IDT_SIZE        256
#define TASK_STACK_SIZE 4 * 1024
#define NR_TASK         4
#define NR_IRQ          16
#define NR_SYS_CALL     1
#define NR_TTY          4
#define NR_TTY_BUF      1024*4

#define HEX 16
#define DEC 10

typedef void  (*hwint_handler)(int irq);//硬件中断处理函数数组
typedef void  (*system_call_var)();//硬件中断处理函数数组

#endif