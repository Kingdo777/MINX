#ifndef _MINX_SYS_CALL_H
#define _MINX_SYS_CALL_H
#include    "process.h"
#include    "ipc.h"

int get_ticks();
// void write(char *buf,int len);
void printx(char *buf);
int sendrec(int function, int src_dest, MESSAGE* m);


int sys_get_ticks();
// int sys_write(uint32_t unused_var1,char *buf,int len,PCB *p);
int sys_printx(uint32_t unused_var1,uint32_t unused_var2,char *s,PCB *pcb);
int sys_sendrec(int function, int src_dest, MESSAGE* m, PCB* p);

int close(int fd);
int open(const char *pathname, int flags);
int read(int fd, void *buf, int count);
int write(int fd, const void *buf, int count);
int unlink(const char * pathname);
int syslog(const char *fmt, ...);
int getCurrentPid();
#endif