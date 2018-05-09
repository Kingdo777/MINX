#include "kliba.h"
#include "mystring.h"
#include "const.h"
#include "global.h"
// 时钟中断处理程序
void    clock_handler(int irq){
    if(Int_reEnter!=0){
        return;
    }
    pcb_ptr++;
    if(pcb_ptr==pcb_table+NR_TASK)
        pcb_ptr=pcb_table;
}