#ifndef _MINX_GLOBAL_H
#define _MINX_GLOBAL_H
#include "protect.h"
#include "process.h"
#include "tty.h"
extern  GDT_PTR     gdt_ptr;
extern  DESCRIPTOR  gdt[GDT_SIZE];

extern  GDT_PTR     idt_ptr;
extern  GATE        idt[IDT_SIZE];

extern  PCB         pcb_table[];
extern  PCB         *pcb_ptr;

extern  TSS         tss;

extern  char        task_stack_A[];
extern  char        task_stack_B[];
extern  char        task_stack_C[];
extern  char        task_stack_tty[];

extern  uint32_t    Int_reEnter;

extern  TASK        task[];
extern  TASK        user_process[];

extern  hwint_handler irq_table[];

extern  system_call_var   system_call_table[];

extern  int ticksCount;//时钟中断次数

extern  TTY tty_table[NR_TTY];
extern  CONSOLE console_table[NR_TTY];
extern  TTY *current_tty;

uint8_t out_char_highLight;

#endif