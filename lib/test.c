#include    "const.h"
#include    "protect.h"
#include    "mystring.h"
#include    "interrupt.h"

void testFunc(){
    puts_asm(0x04,"Hello");
    puts_asm(0x04,"\n");
    puts_asm(0x04,"hello");

}