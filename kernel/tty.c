#include    "const.h"
#include    "global.h"
#include    "keyboard.h"
#include    "mystring.h"
void    task_tty(){
    while(1){
        keyboard_read();
    }
}

void in_process(uint32_t key)
{
    if (!((key & FLAG_EXT)||(key&(~(FLAG_SHIFT_L|FLAG_SHIFT_R|0xff))))){
        putchar(4,(char)key&0x0ff);
    }
}