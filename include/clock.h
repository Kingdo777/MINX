#ifndef _MINX_CLOCK_H
#define _MINX_CLOCK_H
#include "const.h"
#include<stdint.h>


/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             1000  /* clock freq (software settable on IBM-PC) *///1000HZ 表示1秒中产生中断1000次也就是说每1ms一个中断

void 	reset_8253_pit();
void    clock_handler(int irq);
void 	init_clock();


#endif