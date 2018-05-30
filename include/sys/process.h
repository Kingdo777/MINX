#ifndef _MINX_PROCESS_H
#define _MINX_PROCESS_H
#include    "protect.h"
#include    "tty.h"
#include    "fs.h"

void    schedule();

/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	uint64_t	m3l1;
	uint64_t	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;//创建这条消息的进程
	int type;//消息的类型，可以理解为要进行的操作
	union {//消息体，可以接纳如参数或者返回值等
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

typedef struct
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t kernel_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t retaddr;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} STACK_FRAME;
typedef struct proc
{
    STACK_FRAME regs;

    uint16_t ldt_sel;
    DESCRIPTOR ldt[LDT_SIZE];

    int ticks;
    int priority;

    uint32_t pid;

    char *name;

    /*消息机制相关*/
    int  p_flags;/*进程运行状态，值为0时表示可以被调度运行，值为其他是将被阻塞*/

	MESSAGE * p_msg;
	int p_recvfrom;//想要从某进程接受消息的PID
	int p_sendto;//想要发送给某进程消息的PID

	int has_int_msg;//有中断向该进程发送消息

	struct proc * q_sending;//和下面的对象一起构成一个链表，连接了所有想要给该进程发送消息的列表
	struct proc * next_sending;

    TTY *tty;

    /*文件描述符表*/
    FILE_DESC *filp [NR_FILES];

} PCB;

typedef struct{
    uint32_t eip;
    uint32_t topOfStack;
    char *name;
}TASK;

typedef struct
{
    uint32_t backlink;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iobase;

} TSS;


#endif