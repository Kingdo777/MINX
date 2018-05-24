#ifndef _MINX_SYS_CALL_H
#define _MINX_SYS_CALL_H
#include    "process.h"
#include    "ipc.h"

int get_ticks();
void write(char *buf,int len);
void printx(char *buf);
int sendrec(int function, int src_dest, MESSAGE* m);


void sys_get_ticks();
void sys_write(uint32_t unused_var1,char *buf,int len,PCB *p);
void sys_printx(uint32_t unused_var1,uint32_t unused_var2,char *s,PCB *pcb);
void sys_sendrec(int function, int src_dest, MESSAGE* m, PCB* p);
int close(int fd);
int open(const char *pathname, int flags);
#endif