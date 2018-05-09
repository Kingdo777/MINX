#include    "const.h"
void TestA();
void TestB();

// void test_in_asmA();
// void test_in_asmB();

GDT_PTR     gdt_ptr;
DESCRIPTOR  gdt[GDT_SIZE];

GDT_PTR     idt_ptr;
GATE        idt[IDT_SIZE];

PCB         pcb_table[NR_TASK];
PCB         *pcb_ptr;

TSS         tss;

char        task_stack_A[TASK_STACK_SIZE];
char        task_stack_B[TASK_STACK_SIZE];

uint32_t    Int_reEnter;

TASK        task[NR_TASK]={
  {(uint32_t)TestA,(uint32_t)task_stack_A+TASK_STACK_SIZE},
  {(uint32_t)TestB,(uint32_t)task_stack_B+TASK_STACK_SIZE}
};


hwint_handler irq_table[NR_IRQ];