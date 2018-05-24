#include    "const.h"
#include    "protect.h"
#include    "string.h"
#include    "interrupt.h"
#include    "global.h"



void cstart(){
    resetGdt(gdt,&gdt_ptr); 
    setIdt(); 
    setTss();
}