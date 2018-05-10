#include "kliba.h"
#include "mystring.h"
#include "global.h"
#include "clock.h"
#include  <stdint.h>
// 时钟中断处理程序
void    clock_handler(int irq){
    ticksCount++;
    if(Int_reEnter!=0){
        return;
    }
    pcb_ptr++;
    if(pcb_ptr==pcb_table+NR_TASK)
        pcb_ptr=pcb_table;
}

//初始化时钟中断设备PIT
void reset_8253_pit(){
    out_port(TIMER_MODE, RATE_GENERATOR);
    out_port(TIMER0, (uint8_t) (TIMER_FREQ/HZ) );
    out_port(TIMER0, (uint8_t) ((TIMER_FREQ/HZ) >> 8));
}