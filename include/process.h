#ifndef _MINX_PROCESS_H
#define _MINX_PROCESS_H
#include    "protect.h"

void    shcdule();

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
typedef struct
{
    STACK_FRAME regs;

    uint16_t ldt_sel;
    DESCRIPTOR ldt[LDT_SIZE];

    int ticks;
    int priority;

    uint32_t pid;
    char pName[16];
} PCB;

typedef struct{
    uint32_t eip;
    uint32_t topOfStack;
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