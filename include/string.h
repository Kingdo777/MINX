#ifndef _MINX_MYSTRING_H
#define _MINX_MYSTRING_H
#include <stdint.h>
#include "tty.h"
typedef  char *     va_list;

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

uint32_t strlen(const char *s);
void strcpy(char *des,char *src);
void strcat(char *des,char *src);

int printf(const char*fmt,...);
int printl(const char*fmt,...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf,const char *fmt,va_list args);


void panic(const char *fmt, ...);

int memcmp(const void * s1, const void *s2, int n);

int strcmp(const char * s1, const char *s2);
#endif