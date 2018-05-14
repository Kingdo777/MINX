#ifndef _MINX_TTY_H
#define _MINX_TTY_H

#include <stdint.h>

//每个tty都有一个console,console可以理解为tty的显示器，主要是显示用户输入的指令和用户要求显示的信息
//键盘中断的任务只有一个那就是将键盘的scanCode放到键盘缓冲区当中，tyy将不断的执行tty_read和tty_write指令
//其中tty_read负责将scancode从键盘缓冲区中读取出来，然后解码scancode获取一个Key值，并将Key交给函数in_process
//in_process将分析处理key，如果key是一个可打印的字符，那么inprocess将会把字符写到tty字符缓冲区当中
//tty_write的作用就是读取tty字符缓冲区，将字符显示到console

typedef struct{
    uint32_t original_addr;//对应显存位置
    uint32_t current_start_addr;//上下翻页后对应显存的起始位置
    uint32_t v_mem_limit;//占用显存的大小,其值为显存总大小除tty个数
    uint32_t cursor;//光标位置
    uint32_t cursor_MAX;//光标最大显示位置（即光标当前位置）
    uint32_t cursor_LOW;//光标最小显示位置（即$(#)后面的位置）
}CONSOLE;

typedef struct
{
    uint32_t tty_buf[NR_TTY_BUF];
    uint32_t *usable;  //指向下一个可用buf
    uint32_t *pending; //指向下一个要处理的code
    uint32_t count;    //缓冲区中的数据

    CONSOLE *p_console;
} TTY;

void tty_read();
void tty_write();
void in_process(uint32_t key);
void init_tty(TTY *tty);
void task_tty();
void init_screen(TTY *tty);
void set_v_start_addr(uint32_t addr);
void switch_console(int num);

#endif