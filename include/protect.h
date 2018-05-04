#ifndef _MINX_PROTECT_H
#define _MINX_PROTECT_H

#include    "const.h"

void resetGdt(DESCRIPTOR gdt[],GDT_PTR *gdt_ptr);
void init_descriptor(DESCRIPTOR *des,uint32_t base,uint32_t limit,uint16_t attr);
void setTss();
#endif