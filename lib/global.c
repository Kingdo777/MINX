#include "const.h"
#include "protect.h"
#include "process.h"
#include "system_call.h"
#include "tty.h"
#include <stdint.h>

void TestA();
void TestB();
void TestC();
void task_tty();


void test_in_asmA();
void test_in_asmB();

GDT_PTR gdt_ptr;
DESCRIPTOR gdt[GDT_SIZE];

GDT_PTR idt_ptr;
GATE idt[IDT_SIZE];

PCB pcb_table[NR_TASK + NR_USER_PROCESS];
PCB *pcb_ptr;

TSS tss;

char task_stack_A[TASK_STACK_SIZE];
char task_stack_B[TASK_STACK_SIZE];
char task_stack_C[TASK_STACK_SIZE];
char task_stack_tty[TASK_STACK_SIZE];

uint32_t Int_reEnter;

TASK task[NR_TASK] = 
{
    {(uint32_t)task_tty, (uint32_t)task_stack_tty + TASK_STACK_SIZE}
};

TASK user_process[NR_USER_PROCESS]=
{
  {(uint32_t)TestA, (uint32_t)task_stack_A + TASK_STACK_SIZE},
  {(uint32_t)TestB, (uint32_t)task_stack_B + TASK_STACK_SIZE},
  {(uint32_t) TestC, (uint32_t)task_stack_C + TASK_STACK_SIZE}
};

hwint_handler irq_table[NR_IRQ];

system_call_var system_call_func_table[NR_SYS_CALL] = {
    sys_get_ticks,
    sys_write,
    sys_printx
};

int ticksCount; //时钟中断次数

TTY tty_table[NR_TTY];
CONSOLE console_table[NR_TTY];
TTY *current_tty;

uint8_t out_char_highLight=0x07;