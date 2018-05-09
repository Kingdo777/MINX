#ifndef _MINX_GLOBAL_H
#define _MINX_GLOBAL_H

extern  GDT_PTR     gdt_ptr;
extern  DESCRIPTOR  gdt[GDT_SIZE];

extern  GDT_PTR     idt_ptr;
extern  GATE        idt[IDT_SIZE];

extern  PCB         pcb_table[NR_TASK];
extern  PCB         *pcb_ptr;

extern  TSS         tss;

extern  char        task_stack_A[];
extern  char        task_stack_B[];

extern  uint32_t clockInt_reEnter;

extern  TASK        task[];
#endif