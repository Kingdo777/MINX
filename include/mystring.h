#ifndef _MINX_MYSTRING_H
#define _MINX_MYSTRING_H
#include <stdint.h>
#include "tty.h"

//字符串函数

void memcpy(void*dest,void*src,uint32_t size);

void puts_asm(uint8_t attr,char *s);

void putchar_asm(uint8_t attr,char s);

char *itoa(int num,char *str,int show_mode);

void memset(void *s, uint8_t ch, uint32_t n);

void memset_word(void *s, uint16_t word, uint32_t n);

void NL();

void putNum(int num,int mode);

void write_cursor(int);

int read_cursor();

void putchar_c(TTY *tty, char s);//由ttc.c中定义

void putchar(char c);
void puts(char *);

uint32_t strlen(char *s);
void strcpy(char *des,char *src);
void strcat(char *des,char *src);

int printf(const char*fmt,...);

void panic(const char *fmt, ...);

#endif