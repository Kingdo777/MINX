#ifndef _MINX_INTERRUPT_H
#define _MINX_INTERRUPT_H

void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags);
void    setIdt();
void	set_irq_table(int irq,hwint_handler handler);

#endif