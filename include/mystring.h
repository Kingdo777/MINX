#ifndef _MINX_MYSTRING_H
#define _MINX_MYSTRING_H
#include    <stdint.h>
//字符串函数

void memcpy(void*dest,void*src,uint32_t size);

void puts(uint8_t attr,char *s);

void putchar(uint8_t attr,char s);

#endif