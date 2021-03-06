#ifndef _MINX_IPC_H
#define _MINX_IPC_H
#include <stdint.h>
#include "process.h"

void block(PCB* p);
void unblock(PCB* p);
int  msg_send(PCB* current, int dest, MESSAGE* m);
int  msg_receive(PCB* current, int src, MESSAGE* m);
int  deadlock(int src, int dest);
int  getpid(PCB *p);
int send_recv(int ipc_type, int dest_process_pid, MESSAGE* msg);
void dump_proc(PCB* p);
void dump_msg(const char * title, MESSAGE* m);
void reset_msg(MESSAGE* p);
void inform_int(int task_nr);

#endif