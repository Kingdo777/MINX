#include    "const.h"
#include    "protect.h"
#include    "mystring.h"


void resetGdt(DESCRIPTOR gdt[],GDT_PTR *gdt_ptr){
    memcpy((void *)gdt,(void *)(*((uint32_t *)(&gdt_ptr->gdt_base))),(uint32_t)(gdt_ptr->gdt_limit)+1);
    *((uint32_t*)(&gdt_ptr->gdt_base))=(uint32_t)(gdt);
}