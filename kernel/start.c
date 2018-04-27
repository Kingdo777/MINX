#include    "const.h"
#include    "protect.h"
#include    "mystring.h"
#include    "interrupt.h"

GDT_PTR     gdt_ptr;
DESCRIPTOR  gdt[GDT_SIZE];

GDT_PTR     idt_ptr;
GATE        idt[IDT_SIZE]={0};

void cstart(){
    resetGdt(gdt,&gdt_ptr); 
    setIdt(); 
}