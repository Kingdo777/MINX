#ifndef _MINX_KLIBA_H
#define _MINX_KLIBA_H
#include<stdint.h>

void    out_port(uint16_t port,uint8_t value);
uint8_t in_port(uint16_t port);
void    delay(int time);
void    disable_irq(int irq);
void    enable_irq(int irq);

#endif