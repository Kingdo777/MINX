#include "const.h"
#include "global.h"
#include "mystring.h"
#include "process.h"
#include "kliba.h"
//此函数用于实现system_call_table中的函数
int sys_get_ticks(){
    return ticksCount;
}

void sys_write(uint32_t unused_var1,char *buf,int len,PCB *p){
    for(int i=0;i<len;i++){
        // putchar_c(current_tty,buf[i]);        
        putchar_c(p->tty,buf[i]);
    }
}
int sys_printx(uint32_t unused_var1,uint32_t unused_var2,char *s,PCB *pcb){
    const char * p;
	char ch;

	char reenter_err[] = "?Int_reEnter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;


	/**
	 * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
	 * If this happens in Ring 0, no linear-physical address mapping
	 * is needed.
	 *
	 * @attention The value of `Int_reEnter' is tricky here. When
	 *   -# printx() is called in Ring 0
	 *      - Int_reEnter > 0. When code in Ring 0 calls printx(),
	 *        an `interrupt re-enter' will occur (printx() generates
	 *        a software interrupt). Thus `Int_reEnter' will be increased
	 *        by `kernel.asm::save' and be greater than 0.
	 *   -# printx() is called in Ring 1~3
	 *      - Int_reEnter == 0.
	 */
	if (Int_reEnter >= 0)  /* printx() called in Ring<0~3> */
		p = s;
	else	/* this should NOT happen */
		p = reenter_err;

	/**
     * 如果要打印的字符串含有标志位MAG_CH_PANIC，或者在当前进程是TASK且含有标志MAG_CH_ASSERT的时候
     * 将在屏幕中打印错误信息，并且停机，否则正常打印该字符串，若在user_process下产生MAG_CH_ASSERT
     * 将会正常打印，对该进程的处理将会交给更高层
	 */
	// if ((*p == MAG_CH_PANIC) ||
	//     (*p == MAG_CH_ASSERT && pcb < &pcb_table[NR_TASK])) {
    //     close_hardInt();
	// 	// char * v = (char*)V_MEM_BASE;
	// 	// const char * q = p + 1; /* +1: skip the magic char */

	// 	// while (v < (char*)(V_MEM_BASE + V_MEM_SIZE)) {
	// 	// 	*v++ = *q++;
	// 	// 	*v++ = RED_CHAR;
	// 	// 	if (!*q) {
	// 	// 		while (((int)v - V_MEM_BASE) % (SCR_WIDTH * 16)) {
	// 	// 			/* *v++ = ' '; */
	// 	// 			v++;
	// 	// 			*v++ = GRAY_CHAR;
	// 	// 		}
	// 	// 		q = p + 1;
	// 	// 	}
	// 	// }

	// 	__asm__ __volatile__("hlt");
	// }
	p = "24323423424";
	while ((ch = *p++) != 0) {
		if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
			continue; /* skip the magic char */
		putchar_c(pcb->tty, ch);
	}

	return 0;
}