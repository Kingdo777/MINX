#ifndef _MINX_PROTECT_H
#define _MINX_PROTECT_H

#include "type.h"
typedef struct{
    uint16_t    limit_low;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     attr_low;//TYPE(4),S(1),DPL(2),P(1)
    uint8_t     attr_high;//limit_hight(4),0(1),D/B(1),G(1)
    uint8_t     base_high;
}DESCRIPTOR;

typedef struct{//此结构体的设计中将gdt_base划分为了两个16字节主要是考虑了字节对其，或者说是避免字节对其的影响
    uint16_t    gdt_limit;
    uint16_t    gdt_base;
    uint16_t    gdt_base_high;
}GDT_PTR;

#endif