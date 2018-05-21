#ifndef _MINX_SYS_CALL_H
#define _MINX_SYS_CALL_H
#include    "process.h"

int get_ticks();
void write(char *buf,int len);
void printx(char *buf);

void sys_get_ticks();
void sys_write(uint32_t unused_var1,char *buf,int len,PCB *p);
void sys_printx(uint32_t unused_var1,uint32_t unused_var2,char *s,PCB *pcb);
#endif