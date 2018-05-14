#include "process.h"
#include "global.h"

void    shcdule(){
    PCB *p;
    int great_ticks=0;
    while(!great_ticks){
        for(p=pcb_table;p<pcb_table+NR_TASK+NR_USER_PROCESS;p++){
            if(p->ticks>great_ticks){
                great_ticks=p->ticks;
                pcb_ptr=p;
            }
        }
        if(!great_ticks){//当下所有的进程都已经将自己的时间片执行完毕
            for(p=pcb_table;p<pcb_table+NR_TASK+NR_USER_PROCESS;p++){
                p->ticks=p->priority;
            }
        }
    }
}