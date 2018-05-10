//time为毫秒数
#include    "system_call.h"
#include    "clock.h"
#include    <stdint.h>

void delay(unsigned int time)
{
    unsigned int t=get_ticks();
    while((get_ticks()-t)<time)
        ;
}