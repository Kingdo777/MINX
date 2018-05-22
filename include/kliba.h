#ifndef _MINX_KLIBA_H
#define _MINX_KLIBA_H
#include <stdint.h>

void out_port(uint16_t port, uint8_t value);
uint8_t in_port(uint16_t port);
void delay(int time);
void disable_irq(int irq);
void enable_irq(int irq);
void open_hardInt();
void close_hardInt();
void hltf();
void set_out_char_highLight(uint8_t m);
void assertion_failure(char *exp, char *file, char *base_file, int line);
void spin(char *func_name);
int get_ticks_by_message();
void port_read(uint16_t port, void* buf, int n);
void port_write(uint16_t port, void* buf, int n);

#endif