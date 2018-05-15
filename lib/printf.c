#include "global.h"
#include "mystring.h"
#include "const.h"
#include "system_call.h"
typedef  char *     va_list;
#define get_arg_list(fmt)   (va_list)((char*)(&fmt)+4)
int vsprintf(char *buf,const char *fmt,va_list args);

int printf(const char*fmt,...){
    int len;
    char buf[1024];
    va_list args=get_arg_list(fmt);
    // putNum(args,16);
    len=vsprintf(buf,fmt,args);
    write(buf,len);
    return len;
}

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
            default:break;
        }
    }
    return p-buf;
}


void putchar(char c)
{
    char ch = c;
    write(&ch, 1);
}
void puts(char *s){
    char ch;
    while((ch=*(s++))!='\0')
        putchar(ch);
}
void NL()
{
    putchar('\n');
}
void putNum(int num, int mode)
{
    char s[40];
    puts(itoa(num, s, mode));
}