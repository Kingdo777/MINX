#include "kliba.h"
#include "mystring.h"
#include "const.h"
#include "global.h"
// 时钟中断处理程序
void    clock_handler(){
    if(++pcb_ptr==pcb_ptr+NR_TASK)
        pcb_ptr=pcb_table;
}