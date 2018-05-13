#include "const.h"
#include "global.h"
#include "keyboard.h"
#include "mystring.h"
#include "tty.h"
#include "kliba.h"

void task_tty()
{
    TTY *tty=tty_table;
    current_tty=tty_table;
    for(int i=0;i<NR_TTY;i++){
        init_tty(tty+i);
    }
    while (1)
    {
        tty_read();
        tty_write();
    }
}

void tty_read(){
    keyboard_read();
}

void tty_write(){
    int key;
    char c;
    if(current_tty->count>0){
        key=*(current_tty->pending);
        c=(char)key&0x0ff;
        if(current_tty->pending++==current_tty->tty_buf+NR_TTY_BUF)
            current_tty->pending=current_tty->tty_buf;
        current_tty->count--;
        putchar(4,c);
    }
}

void in_process(uint32_t key)
{
    if (!((key & FLAG_EXT) || (key & (~(FLAG_SHIFT_L | FLAG_SHIFT_R | 0xff)))))
    {
        //如果按下的是可打印字符，且除shift键之外无其它控制信息，那么将其写入tty缓冲区
        if(current_tty->count<NR_TTY_BUF){
            *(current_tty->usable)=key;
            if(current_tty->usable++==current_tty->tty_buf+NR_TTY_BUF)
                current_tty->usable=current_tty->tty_buf;
            current_tty->count++;
        }
    }else{
        switch(key&0x1ff){
            case F1:
            case F2:
            case F3:
            case F4:
            case F5:
            case F6:
            case F7:
            case F8:
            case F9:
            case F10:
            case F11:
            case F12:
                if(key&FLAG_CTRL_L||key&FLAG_CTRL_R)
                    switch_console(key&0x1ff-F1);
                break;
            default:break;
        }
    }
}

void switch_console(int num){
    if(num<0||num>=NR_TTY)
        return;
    TTY tty=tty_table[num];
    *(current_tty)=tty;
    write_cursor(tty.p_console->cursoe);
    set_v_start_addr(tty.p_console->current_start_addr);
}

void set_v_start_addr(uint32_t addr){
    close_hardInt();
    out_port(0x3c4,0x0c);//索引端口（0x3c4），索引到起始地址高位端口（0x0c）
    out_port(0x3c5,addr>>8&&0xff);//数据端口（0x3c5）
    out_port(0x3c4,0x0D);//索引端口（0x3c4），索引到起始地址高位端口（0x0D）
    out_port(0x3c5,addr&&0xff);//数据端口（0x3c5）

    open_hardInt();
}

void init_tty(TTY *tty){
    tty->count=0;
    tty->usable=tty->pending=tty->tty_buf;
    init_screen(tty);
}

void init_screen(TTY *tty){
    CONSOLE *console=console_table+(tty-tty_table);
    tty->p_console=console;
    
    
}