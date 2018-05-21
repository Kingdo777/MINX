#include "kliba.h"
#include "global.h"
#include "process.h"
#include "clock.h"
#include "interrupt.h"
#include  <stdint.h>
// 时钟中断处理程序
void    clock_handler(int irq){
    ticksCount++;
    pcb_ptr->ticks--;
    if(Int_reEnter!=0){
        return;
    }
    if(pcb_ptr->ticks>0){
        return;
    }else{
        schedule();
    }
    // pcb_ptr++;
    // if(pcb_ptr==pcb_table+NR_TASK)
    //     pcb_ptr=pcb_table;
}

//初始化时钟中断设备PIT
void reset_8253_pit(){
    out_port(TIMER_MODE, RATE_GENERATOR);
    out_port(TIMER0, (uint8_t) (TIMER_FREQ/HZ) );
    out_port(TIMER0, (uint8_t) ((TIMER_FREQ/HZ) >> 8));
}

void init_clock(){
    reset_8253_pit();
    set_irq_table(CLOCK_IRQ,clock_handler);    
}