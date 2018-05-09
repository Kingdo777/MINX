#include "kliba.h"
#include "mystring.h"
#include "const.h"
#include "global.h"

void restart();
void test_in_asm();

void kernelMain()
{
    PCB *p;
    for(int i=0;i<NR_TASK;i++){
        p=&pcb_table[i];
        p->ldt_sel = SELECTOR_LDT;
        p->regs.cs = SELECTOR_USER_CODE_4G;
        p->regs.ds = SELECTOR_USER_DATA_4G;
        p->regs.es = SELECTOR_USER_DATA_4G;
        p->regs.gs = SELECTOR_USER_DATA_4G;
        p->regs.fs = SELECTOR_USER_DATA_4G;
        p->regs.ss = SELECTOR_USER_DATA_4G;
        p->regs.eip = (uint32_t)task[i].eip;
        p->regs.esp = (uint32_t)task[i].topOfStack;
        p->regs.eflags = 0x1202; //IF=1,IOPL=1,第二位恒为1
    }
    pcb_ptr = pcb_table;
    clockInt_reEnter=-1;//时钟中断重入检测
    restart();
    while (1)
        ;
}

void TestA()
{
    while(1){
        putchar(4,'A');
        delay(1);
    }
}

void TestB()
{
    while(1){
        putchar(4,'B');
        delay(1);
    }
}