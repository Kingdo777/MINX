#include    "../include/type.h"
#include    "../include/const.h"
#include    "../include/protect.h"
#include    "../include/mystring.h"

GDT_PTR     gdt_ptr;
DESCRIPTOR  gdt[GDT_SIZE];

void resetGdt(){
    puts(0x05,"I am in C\n");
    memcpy((void *)gdt,(void *)(*((uint32_t *)(&gdt_ptr.gdt_base))),(uint32_t)(gdt_ptr.gdt_limit)+1);
    *((uint32_t*)(&gdt_ptr.gdt_base))=(uint32_t)(gdt);
}