#include <stdint.h>
#include "const.h"
#include "global.h"
#include "keyboard.h"
#include "tty.h"
#include "mystring.h"
void putchar(uint8_t attr, char s);
void puts(uint8_t attr, char *s);

void putchar_c(uint8_t attr, char s){
    //考虑控制台之后的putchar
    TTY *tty=current_tty;
    int cursor=tty->p_console->cursor;
    char *v_mem_addr=(char*)V_MEM_BASE+2*tty->p_console->cursor;
    int has_null=0;//改变量用于退格，若退格前遇到的是\n，那么将光标恢复到第一个字符前，否则是向前退一步
    switch(s){
        case '\n':
        case '\r':
            for(int i=0;i<80&&i<((cursor/80+1)*80-cursor);i++){
                *(v_mem_addr++)='\0';
                *(v_mem_addr++)=7;
            }
            cursor=(cursor/80+1)*80;
            break;
        case '\b':
            for(int i=0;i<80&&((((*(v_mem_addr-1))==7)&&((*(v_mem_addr-2))=='\0')));i++){
                v_mem_addr-=2;
                cursor--;
                has_null=1;
            }
            if(!has_null){
                *(--v_mem_addr)=7;        
                *(--v_mem_addr)=' ';
                cursor--;                
            }
            break;
        default:
            *(v_mem_addr++)=s;
            *(v_mem_addr++)=attr;
            cursor++;
            break;
    }
    if(cursor>=tty->p_console->original_addr+tty->p_console->v_mem_limit){
        //这个时候已经超出了显存的最大内容，我们只能覆盖最开始的内容
        memcpy((char*)V_MEM_BASE+2*tty->p_console->original_addr+80*2,(char *)tty->p_console->original_addr,80*2);
        cursor-=80;
    }
    if(cursor>=tty->p_console->current_start_addr+80*25){
        tty->p_console->current_start_addr+=80;
    }else if(cursor<tty->p_console->current_start_addr){
        tty->p_console->current_start_addr-=80;
        if(tty->p_console->current_start_addr<=tty->p_console->original_addr)
            cursor=tty->p_console->current_start_addr=tty->p_console->original_addr;
    }
    tty->p_console->cursor=cursor;
    set_v_start_addr(tty->p_console->current_start_addr);
    write_cursor(cursor);
}
void puts_c(uint8_t attr, char *s){
    char *p=s;
    while(*p!='\0')
        putchar(attr,*(p++));
}
char *itoa(int num, char *str, int show_mode)
{
    int n;
    char p[32];
    char *q = str, *q1 = p;

    if (show_mode == HEX)
    {
        *q++ = '0';
        *q++ = 'x';
    }
    if (!num)
        *q++ = '0';
    else
        while (num)
        {
            *q1++ = (num % show_mode + (num % show_mode > 9 ? 'A' - 10 : '0'));
            num /= show_mode;
        }
    while (q1 != p)
        *q++ = *(--q1);
    *q++ = '\0';
    return str;
}
void NL()
{
    putchar_c(0, '\n');
}
void putNum(uint8_t attr,int num,int mode){
    char s[40];
    puts_c(attr,itoa(num,s,mode));
}