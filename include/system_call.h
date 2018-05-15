#ifndef _MINX_SYS_CALL_H
#define _MINX_SYS_CALL_H
#include    "process.h"

int get_ticks();
void write(char *buf,int len);

void sys_get_ticks();
void sys_write(char *buf,int len,PCB *p);
#endif