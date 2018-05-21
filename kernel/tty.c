#include "const.h"
#include "global.h"
#include "keyboard.h"
#include "mystring.h"
#include "tty.h"
#include "kliba.h"

void init_all_tty(){
    TTY *tty = tty_table;
    current_tty = tty_table;
    for (int i = 0; i < NR_TTY; i++)
    {
        init_tty(tty + i);
    }
    init_keyboard();
}

void task_tty()
{ 
    assert(0);
    
    while (1)
    {
        tty_read();
        tty_write();
    }
}

void tty_read()
{
    keyboard_read();
}

void tty_write()
{
    int key;
    char c;
    if (current_tty->count > 0)
    {
        key = *(current_tty->pending);
        c = (char)key & 0x0ff;
        if (current_tty->pending++ == current_tty->tty_buf + NR_TTY_BUF)
            current_tty->pending = current_tty->tty_buf;
        current_tty->count--;
        putchar_c(current_tty,c);
    }
}

void in_process(uint32_t key)
{
    if (!((key & FLAG_EXT) || (key & (~(FLAG_SHIFT_L | FLAG_SHIFT_R | 0xff)))))
    {
        //如果按下的是可打印字符，且除shift键之外无其它控制信息，那么将其写入tty缓冲区
        if (current_tty->count < NR_TTY_BUF)
        {
            *(current_tty->usable) = key;
            if (current_tty->usable++ == current_tty->tty_buf + NR_TTY_BUF)
                current_tty->usable = current_tty->tty_buf;
            current_tty->count++;
        }
    }
    else
    {
        switch (key & 0x1ff)
        {
        case PAGEUP:
            if(current_tty->p_console->current_start_addr>=80){
                current_tty->p_console->current_start_addr-=80;
                if(current_tty->p_console->current_start_addr<current_tty->p_console->original_addr){
                    current_tty->p_console->current_start_addr+=80;
                }else{
                    set_v_start_addr(current_tty->p_console->current_start_addr);
                }
            }
            break;            
            
        case PAGEDOWN:
            current_tty->p_console->current_start_addr+=80;
            if(current_tty->p_console->current_start_addr+80*25>current_tty->p_console->original_addr+current_tty->p_console->v_mem_limit){
                current_tty->p_console->current_start_addr-=80;
            }else{
                set_v_start_addr(current_tty->p_console->current_start_addr);
            }
            break;
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
            if (key & FLAG_CTRL_L || key & FLAG_CTRL_R)
                if (((key & 0x1ff) - F1)!=(current_tty-tty_table))
                    switch_console((key & 0x1ff) - F1);
            break;
        default:
            break;
        }
    }
}

void switch_console(int num)
{
    if (num < 0 || num >= NR_TTY)
        return;
    current_tty=&tty_table[num];
    set_v_start_addr(current_tty->p_console->current_start_addr);
    write_cursor(current_tty->p_console->cursor);
}

void set_v_start_addr(uint32_t addr)
{
    close_hardInt();
    out_port(CRTC_ADDR_REG, START_ADDR_H);
    out_port(CRTC_DATA_REG, ((addr) >> 8) & 0xFF);
    out_port(CRTC_ADDR_REG, START_ADDR_L);
    out_port(CRTC_DATA_REG, (addr)&0xFF);
    open_hardInt();
}

void init_tty(TTY *tty)
{
    tty->count = 0;
    tty->usable = tty->pending = tty->tty_buf;
    init_screen(tty);
}

void init_screen(TTY *tty)
{
    int tty_ntr = tty - tty_table ;
    int v_mem_size = V_MEM_SIZE >> 1; /* 显存总大小 (in WORD) */
    int con_v_mem_size = v_mem_size / NR_TTY;

    CONSOLE *console = console_table + tty_ntr;
    tty->p_console = console;

    tty->p_console->original_addr = tty_ntr * con_v_mem_size;
    tty->p_console->v_mem_limit = con_v_mem_size;
    tty->p_console->current_start_addr = tty->p_console->original_addr;

    /* 默认光标位置在最开始处 */
    tty->p_console->cursor = tty->p_console->original_addr;

    if (tty_ntr == 0)
    {
        /* 第一个控制台沿用原来的光标位置 */
        tty->p_console->cursor = read_cursor();
    }
    else
    {
        // out_char(tty->p_console, tty_ntr + '0');
        // out_char(tty->p_console, '#');
    }

}
/**
 * 清屏幕操作，让current_start_addr与original_addr重合，并将整个tty置为0x0720
 * 0x0720:黑底白字的空格
*/
void clear(){
    current_tty->p_console->cursor=
    current_tty->p_console->current_start_addr=
    current_tty->p_console->original_addr;
    set_v_start_addr(current_tty->p_console->current_start_addr);
    write_cursor(current_tty->p_console->cursor);
    memset_word((void *)V_MEM_BASE+current_tty->p_console->cursor*2,
    0x0720,
    current_tty->p_console->v_mem_limit
    );
}