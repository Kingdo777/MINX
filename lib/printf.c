#include "global.h"
#include "mystring.h"
#include "const.h"
#include "kliba.h"
#include "system_call.h"
typedef  char *     va_list;
#define get_arg_list(fmt)   (va_list)((char*)(&fmt)+4)
int vsprintf(char *buf,const char *fmt,va_list args);
/*======================================================================*
                                 printf
 *======================================================================*/
int printf(const char*fmt,...){
    int len;
    char buf[1024];
    va_list args=get_arg_list(fmt);
    // putNum(args,16);
    len=vsprintf(buf,fmt,args);
    // write(buf,len);
    printx(buf);
    return len;
}
/*======================================================================*
                                 vsprintf
 *======================================================================*/
int vsprintf(char *buf,const char *fmt,va_list args){
    char *p=buf;
    char *s;
    char  tmp[256];
    va_list next_arg=args;
    for(;*fmt;fmt++){
        if(*fmt!='%'){
            *p++ = *fmt;
            continue;
        }
        fmt++;
        switch(*fmt){
            case '%':
                *p++ ='%';
                break;
            case 's':
                s=(char *)(*((int *)next_arg));
                strcpy(p,s);
                p+=strlen(s);
                next_arg+=4;
                break;
            case 'd':
                itoa(*((int *)next_arg),tmp,10);
                // itoa(15,tmp,10);
                strcpy(p,tmp);
                p+=strlen(tmp);
                next_arg+=4;
                break;
            case 'x':
                itoa(*((int *)next_arg),tmp,16);
                // itoa(15,tmp,10);
                strcpy(p,tmp);
                p+=strlen(tmp);
                next_arg+=4;
                break;
            case 'c':
                *p++=*((char *)next_arg);
                next_arg+=4;
                break;
            default:break;
        }
    }
    *p='\0';
    return p-buf;
}
/*======================================================================*
                                 putchar
 *======================================================================*/
void putchar(char c)
{
    printf("%c",c);
}
/*======================================================================*
                                 puts
 *======================================================================*/
void puts(char *s){
    printf("%s",s);
}
/*======================================================================*
                                 NL
 *======================================================================*/
void NL()
{
    putchar('\n');
}
/*======================================================================*
                                 putNum
 *======================================================================*/
void putNum(int num, int mode)
{
    char s[40];
    puts(itoa(num, s, mode));
}

/*======================================================================*
                                 sprintf
 *======================================================================*/
int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}
/*****************************************************************************
 *                           panic
 * 该函数类似于assert，其表示发生严重系统错误，直接导致停机处理
 ****************************************************************************/

void panic(const char *fmt, ...)
{
	int i;
	char buf[1024];
	va_list args=get_arg_list(fmt);
	i = vsprintf(buf, fmt, args);

	set_out_char_highLight(4);	    
	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);
	set_out_char_highLight(7);    
	/* should never arrive here */
	__asm__ __volatile__("ud2");
}