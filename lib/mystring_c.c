#include <stdint.h>
#include "const.h"
#include "global.h"
#include "keyboard.h"
#include "tty.h"
#include "string.h"


void putchar_c(TTY *tty, char s){
    //考虑控制台之后的putchar
    // TTY *tty=current_tty;
    int cursor=tty->p_console->cursor;
    char *v_mem_addr=(char*)V_MEM_BASE+2*tty->p_console->cursor;
    int has_null=0;//改变量用于退格，若退格前遇到的是\n，那么将光标恢复到第一个字符前，否则是向前退一步
    switch(s){
        case '\n':
        case '\r':
            cursor-=current_tty->p_console->current_start_addr;//这个操作的目的在于让cursor无论在那个console，都可以利用80来计算需要填充的字符
            for(int i=0;i<80&&i<((cursor/80+1)*80-cursor);i++){
                *(v_mem_addr++)='\0';
                *(v_mem_addr++)=7;
            }
            cursor=(cursor/80+1)*80;
            cursor+=current_tty->p_console->current_start_addr;
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
            *(v_mem_addr++)=out_char_highLight;
            cursor++;
            break;
    }
    if(cursor>=tty->p_console->original_addr+tty->p_console->v_mem_limit){
        //这个时候已经超出了显存的最大内容，我们只能覆盖最开始的内容
        memcpy((char*)V_MEM_BASE+2*tty->p_console->original_addr,
        (char*)V_MEM_BASE+2*tty->p_console->original_addr+80*2,
        tty->p_console->v_mem_limit*2-80*2);
        cursor=tty->p_console->original_addr+tty->p_console->v_mem_limit-80;
        memset((char*)V_MEM_BASE+cursor*2, 0, 80*2);
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
uint32_t strlen(const char *s){
    int i=0;
    while(*s++)
        i++;
    return i;
}
void strcpy(char *des,char *src){
    while(*src){
        *des++ = *src++;
    }
    *des='\0';
}
void strcat(char *des,char *src){
    uint32_t len=strlen(des);
    strcpy(&des[len],src);
}
int memcmp(const void * s1, const void *s2, int n)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = (const char *)s1;
	const char * p2 = (const char *)s2;
	int i;
	for (i = 0; i < n; i++,p1++,p2++) {
		if (*p1 != *p2) {
			return (*p1 - *p2);
		}
	}
	return 0;
}
int strcmp(const char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = s1;
	const char * p2 = s2;

	for (; *p1 && *p2; p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}