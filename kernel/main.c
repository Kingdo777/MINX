#include "kliba.h"
#include "mystring.h"
#include "const.h"
#include "global.h"
#include "interrupt.h"
#include "system_call.h"
#include "clock.h"
#include "keyboard.h"

void restart();
void test_in_asm();

void kernelMain()
{
    PCB *p;
    for (int i = 0; i < NR_TASK; i++)
    {
        p = &pcb_table[i];
        p->ldt_sel = SELECTOR_LDT;
        p->pid = i;
        p->regs.cs = SELECTOR_TASK_CODE_4G;
        p->regs.ds = SELECTOR_TASK_DATA_4G;
        p->regs.es = SELECTOR_TASK_DATA_4G;
        p->regs.gs = SELECTOR_TASK_DATA_4G;
        p->regs.fs = SELECTOR_TASK_DATA_4G;
        p->regs.ss = SELECTOR_TASK_DATA_4G;
        p->regs.eip = (uint32_t)task[i].eip;
        p->regs.esp = (uint32_t)task[i].topOfStack;
        p->regs.eflags = 0x1202; //IF=1,IOPL=1,第二位恒为1
        // p->regs.eflags = 0x0202; //IF=1,IOPL=0,第二位恒为1
        pcb_table[i].ticks = pcb_table[i].priority = 30;
    }
    //为每个进程初始化权值
    // pcb_table[0].ticks=pcb_table[0].priority=30;
    // pcb_table[1].ticks=pcb_table[1].priority=30;
    // pcb_table[2].ticks=pcb_table[2].priority=30;
    // pcb_table[3].ticks=pcb_table[2].priority=30;

    pcb_ptr = pcb_table;
    Int_reEnter = 0; //中断重入检测
    ticksCount = 0;  //很奇怪，我在global.c中对ticksCount的初始化无效

    init_clock();

    restart();
    while (1)
        ;
}

void TestA()
{
    while (1)
    {
        // puts(4,"A.");
        // putNum(4,get_ticks(),DEC);
        delay(10);
    }
}

void TestB()
{
    while (1)
    {
        // puts(5,"B.");
        delay(10);
    }
}

void TestC()
{
    while (1)
    {
        // puts(6,"C.");
        delay(10);
    }
}