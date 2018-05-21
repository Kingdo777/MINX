//time为毫秒数
#include    "system_call.h"
#include    "clock.h"
#include    "const.h"
#include    "global.h"
#include    "mystring.h"
#include    "ipc.h"
#include    <stdint.h>

int get_ticks_by_message(){
	MESSAGE m;
	reset_msg(&m);
	m.type=GET_TICKS;
	send_recv(BOTH,TASK_SYS,&m);
	return m.RETVAL;
}

void delay(unsigned int time)
{
    unsigned int t=get_ticks();
    while((get_ticks()-t)<time);
	// unsigned int t=get_ticks_by_message();
    // while((get_ticks_by_message()-t)<time);
}
void set_out_char_highLight(uint8_t m){
    out_char_highLight=m;
}


/*****************************************************************************
 *                                spin
 * 函数用于处理出现问题的用户进程，暂时设置其为让问题进程无限死循环
 *****************************************************************************/
void spin(char * func_name)
{
	set_out_char_highLight(4);
	printl("\nspinning in %s ...\n", func_name);
	set_out_char_highLight(7);
	while (1) {}
}


/*****************************************************************************
 *                           assertion_failure
 *************************************************************************//**
 * Invoked by assert().
 *
 * @param exp       The failure expression itself.
 * @param file      __FILE__
 * @param base_file __BASE_FILE__
 * @param line      __LINE__
 *****************************************************************************/
void assertion_failure(char *exp, char *file, char *base_file, int line)
{
	set_out_char_highLight(4);
	putchar('\n');		
	printl("%cassert(%s) failed: file: %s, base_file: %s, line:%d",
	       MAG_CH_ASSERT,
	       exp, file, base_file, line);
	set_out_char_highLight(7);
	/**
	 * 当跳入该函数时说明已经产生了错误，那么此时分两种情况分别处理，一种是此时为TASK进程，那么会在printx系统调用中处理
     * 另一种为用户进程，我们将使用spin函数处理之。
	 */
	spin("assertion_failure()");

	/* should never arrive here */
        __asm__ __volatile__("ud2");
}