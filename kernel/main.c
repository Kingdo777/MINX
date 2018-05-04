#include    "kliba.h"
#include    "mystring.h"
#include    "const.h"
#include    "global.h"

void restart();

extern  PCB     pcb_table[NR_TASK];
extern  TSS     tss_core;

void kernelMain(){
    PCB *p=pcb;
    p->ldt_sel=SELECTOR_LDT;
    p->regs.cs=SELECTOR_USER_CODE_4G;
    p->regs.ds=SELECTOR_USER_DATA_4G;
    p->regs.es=SELECTOR_USER_DATA_4G;
    p->regs.gs=SELECTOR_USER_DATA_4G;
    p->regs.fs=SELECTOR_USER_DATA_4G;
    p->regs.ss=SELECTOR_USER_DATA_4G;
    p->regs.eip=(uint32_t)TestA;
    p->regs.esp=(uint32_t)task_stack+TASK_STACK_SIZE;
    p->regs.eflags=0x3202;//IF=1,IOPL=3,第二位恒为1
    pcb_ptr=p;
    restart();
    while(1); 
}

void TestA(){
    while(1){
        putchar(0x04,'A');
        delay(1);
    }
}