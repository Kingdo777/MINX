#include    "const.h"


GDT_PTR     gdt_ptr;
DESCRIPTOR  gdt[GDT_SIZE];

GDT_PTR     idt_ptr;
GATE        idt[IDT_SIZE];

PCB         pcb_table[NR_TASK];
PCB         *pcb_ptr;

TSS         tss;

char        task_stack[TASK_STACK_SIZE];

uint32_t clockInt_reEnter;