#include "kliba.h"
#include "mystring.h"
#include "const.h"
#include "global.h"

void restart();
void TestA();
void test_in_asm();

void kernelMain()
{
    PCB *p = pcb_table;
    p->ldt_sel = SELECTOR_LDT;
    p->regs.cs = SELECTOR_USER_CODE_4G;
    p->regs.ds = SELECTOR_USER_DATA_4G;
    p->regs.es = SELECTOR_USER_DATA_4G;
    p->regs.gs = SELECTOR_USER_DATA_4G;
    p->regs.fs = SELECTOR_USER_DATA_4G;
    p->regs.ss = SELECTOR_USER_DATA_4G;
    p->regs.eip = (uint32_t)test_in_asm;
    p->regs.esp = (uint32_t)task_stack + TASK_STACK_SIZE;
    p->regs.eflags = 0x1202; //IF=1,IOPL=1,第二位恒为1
    pcb_ptr = p;
    restart();
    while (1);
}

void TestA()
{
    while(1){
        puts(4,"I am In TestA\n");
    }
//    for(int i=0;i<100000;i++){
//        putNum(i);
//        putchar(0,'\t');
//    }
}