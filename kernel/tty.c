#include "const.h"
#include "global.h"
#include "keyboard.h"
#include "string.h"
#include "tty.h"
#include "kliba.h"
#include "ipc.h"
#include "stdio.h"
void tty_do_write(TTY* tty, MESSAGE* msg);
void tty_do_read(TTY* tty, MESSAGE* msg);
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
    MESSAGE msg;
    while (1)
    {
        tty_dev_read();
        tty_dev_write();

        send_recv(RECEIVE, ANY, &msg);

		int src = msg.source;
		assert(src != TASK_TTY);

		TTY* ptty = &tty_table[msg.DEVICE];

		switch (msg.type) {
		case DEV_OPEN:
			reset_msg(&msg);
			msg.type = SYSCALL_RET;
			send_recv(SEND, src, &msg);
			break;
		case DEV_READ:
			tty_do_read(ptty, &msg);
			break;
		case DEV_WRITE:
			tty_do_write(ptty, &msg);
			break;
		case HARD_INT:
			/**
			 * waked up by clock_handler -- a key was just pressed
			 * @see clock_handler() inform_int()
			 */
			key_pressed = 0;
			continue;
		default:
			dump_msg("TTY::unknown msg", &msg);
			break;
		}
    }
}
/*****************************************************************************
 *                                tty_do_read
 *****************************************************************************/
/**
 * Invoked when task TTY receives DEV_READ message.
 *
 * @note The routine will return immediately after setting some members of
 * TTY struct, telling FS to suspend the proc who wants to read. The real
 * transfer (tty buffer -> proc buffer) is not done here.
 * 
 * @param tty  From which TTY the caller proc wants to read.
 * @param msg  The MESSAGE just received.
 *****************************************************************************/
void tty_do_read(TTY* tty, MESSAGE* msg)
{
	/* tell the tty: */
	tty->tty_caller   = msg->source;  /* who called, usually FS */
	tty->tty_procnr   = msg->PROC_NR; /* who wants the chars */
	tty->tty_req_buf  = msg->BUF;/* where the chars should be put */
	tty->tty_left_cnt = msg->CNT; /* how many chars are requested */
	tty->tty_trans_cnt= 0; /* how many chars have been transferred */

	msg->type = SUSPEND_PROC;
	msg->CNT = tty->tty_left_cnt;
	send_recv(SEND, tty->tty_caller, msg);
}


/*****************************************************************************
 *                                tty_do_write
 *****************************************************************************/
/**
 * Invoked when task TTY receives DEV_WRITE message.
 * 
 * @param tty  To which TTY the calller proc is bound.
 * @param msg  The MESSAGE.
 *****************************************************************************/
void tty_do_write(TTY* tty, MESSAGE* msg)
{
	char buf[TTY_OUT_BUF_LEN];
	char * p = (char*)msg->BUF;
	int i = msg->CNT;
	int j;

	while (i) {
		int bytes = min(TTY_OUT_BUF_LEN, i);
		memcpy(buf, (void*)p, bytes);
		for (j = 0; j < bytes; j++)
			putchar_c(tty, buf[j]);
		i -= bytes;
		p += bytes;
	}

	msg->type = SYSCALL_RET;
	send_recv(SEND, msg->source, msg);
}

void tty_dev_read()
{
    keyboard_read();
}

void tty_dev_write()
{
    int key;
    char c;
    TTY *tty=current_tty;
    if (tty->ibuf_cnt > 0)
    {
        char ch = *(tty->ibuf_tail);
		tty->ibuf_tail++;
		if (tty->ibuf_tail == tty->ibuf + NR_TTY_BUF)
			tty->ibuf_tail = tty->ibuf;
		tty->ibuf_cnt--;

		if (tty->tty_left_cnt) {
			if (ch >= ' ' && ch <= '~') { /* printable */
				putchar_c(tty, ch);
				void * p = tty->tty_req_buf +
					   tty->tty_trans_cnt;
				memcpy(p, (void *)&ch, 1);
				tty->tty_trans_cnt++;
				tty->tty_left_cnt--;
			}
			else if (ch == '\b' && tty->tty_trans_cnt) {
				putchar_c(tty, ch);
				tty->tty_trans_cnt--;
				tty->tty_left_cnt++;
			}

			if (ch == '\n' || tty->tty_left_cnt == 0) {
				putchar_c(tty, '\n');
				MESSAGE msg;
				msg.type = RESUME_PROC;
				msg.PROC_NR = tty->tty_procnr;
				msg.CNT = tty->tty_trans_cnt;
				send_recv(SEND, tty->tty_caller, &msg);
				tty->tty_left_cnt = 0;
			}
		}
    }
}

void in_process(uint32_t key)
{
    if (!((key & FLAG_EXT) || (key & (~(FLAG_SHIFT_L | FLAG_SHIFT_R | 0xff)))))
    {
        //如果按下的是可打印字符，且除shift键之外无其它控制信息，那么将其写入tty缓冲区
        if (current_tty->ibuf_cnt < NR_TTY_BUF)
        {
            *(current_tty->ibuf_head) = key;
            if (current_tty->ibuf_head++ == current_tty->ibuf + NR_TTY_BUF)
                current_tty->ibuf_head = current_tty->ibuf;
            current_tty->ibuf_cnt++;
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
    tty->ibuf_cnt = 0;
    tty->ibuf_head = tty->ibuf_tail = tty->ibuf;
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
        // putchar_c(tty->p_console, tty_ntr + '0');
        // putchar_c(tty->p_console, '#');
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