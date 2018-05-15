#include "const.h"
#include "global.h"
#include "mystring.h"
//此函数用于实现system_call_table中的函数
int sys_get_ticks(){
    return ticksCount;
}

void sys_write(char *buf,int len,PCB *p){
    for(int i=0;i<len;i++){
        // putchar_c(current_tty,buf[i]);        
        putchar_c(p->tty,buf[i]);
    }
}