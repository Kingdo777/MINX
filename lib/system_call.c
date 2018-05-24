#include "const.h"
#include "global.h"
#include "string.h"
#include "process.h"
#include "kliba.h"
#include "ipc.h"
#include "stdio.h"
//此函数用于实现system_call_table中的函数
int sys_get_ticks()
{
	return ticksCount;
}

// void sys_write(uint32_t unused_var1, char *buf, int len, PCB *p)
// {
// 	for (int i = 0; i < len; i++)
// 	{
// 		// putchar_c(current_tty,buf[i]);
// 		putchar_c(p->tty, buf[i]);
// 	}
// }
void sys_printx(uint32_t unused_var1, uint32_t unused_var2, char *s, PCB *pcb)
{
	const char *p;
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
	if (Int_reEnter >= 0) /* printx() called in Ring<0~3> */
		p = s;
	else /* this should NOT happen */
		p = reenter_err;

	/**
     * 如果要打印的字符串含有标志位MAG_CH_PANIC，或者在当前进程是TASK且含有标志MAG_CH_ASSERT的时候
     * 将在屏幕中打印错误信息，并且停机，否则正常打印该字符串，若在user_process下产生MAG_CH_ASSERT
     * 将会正常打印，对该进程的处理将会交给更高层
	 */
	//
	if ((*p == MAG_CH_PANIC) || (*p == MAG_CH_ASSERT && pcb < &pcb_table[NR_TASK]))
	{
		p++;
		while ((ch = *p++) != 0)
			putchar_c(pcb->tty, ch);
		close_hardInt();
		__asm__ __volatile__("hlt");
	}
	else
	{
		while ((ch = *p++) != 0)
		{
			if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
				continue; /* skip the magic char */
			putchar_c(pcb->tty, ch);
		}
	}
	return;
}

/*****************************************************************************
 *                                sys_sendrec
 *****************************************************************************/
/**
 * <Ring 0> The core routine of system call `sendrec()'.
 * 
 * @param ipc_type SEND or RECEIVE
 * @param dest_process_pid To/From whom the message is transferred.
 * @param m        Ptr to the MESSAGE body.
 * @param p        The caller proc.
 * 
 * @return Zero if success.
 *****************************************************************************/
int sys_sendrec(int ipc_type, int dest_process_pid, MESSAGE* m, PCB* p)
{
	//我其实没有理解下面这条代码的含义，原文的注释是“make sure we are not in ring0”
	assert(Int_reEnter == 0);	
	//保证目标进程的取值合理
	assert((dest_process_pid >= 0 && dest_process_pid < NR_TASK + NR_USER_PROCESS) ||
	       dest_process_pid == ANY ||
	       dest_process_pid == INTERRUPT);

	int ret = 0;
	int caller = getpid(p);
	m->source = caller;
	//保证收件人和发送人不是同一个
	assert(m->source != dest_process_pid);

	/**
	 * ipc_type还有第三种类型BOTH，该扫作可以看作是先进行SEND然后在RECEIVE，主要应用场景是接受一个返回值，BOTH的处理在函数send_rec中处理，
	 * 因此在这里不需要考虑
	 */
	if (ipc_type == SEND) {
		ret = msg_send(p, dest_process_pid, m);
		if (ret != 0)
			return ret;
	}
	else if (ipc_type == RECEIVE) {
		ret = msg_receive(p, dest_process_pid, m);
		if (ret != 0)
			return ret;
	}
	else {
		panic("{sys_sendrec} invalid ipc_type: "
		      "%d (SEND:%d, RECEIVE:%d).", ipc_type, SEND, RECEIVE);
	}

	return 0;
}

/**
* 下面的系统调用函数都是通过消息机制实现的
*/
/*****************************************************************************
 *                                open
 *****************************************************************************/
/**
 * open/create a file.
 * 
 * @param pathname  The full path of the file to be opened/created.
 * @param flags     O_CREAT, O_RDWR, etc.
 * 
 * @return File descriptor if successful, otherwise -1.
 *****************************************************************************/
int open(const char *pathname, int flags)
{
	MESSAGE msg;

	msg.type	= OPEN;

	msg.PATHNAME	= (void*)pathname;
	msg.FLAGS	= flags;
	msg.NAME_LEN	= strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.FD;
}

/*****************************************************************************
 *                                close
 *****************************************************************************/
/**
 * Close a file descriptor.
 * 
 * @param fd  File descriptor.
 * 
 * @return Zero if successful, otherwise -1.
 *****************************************************************************/
int close(int fd)
{
	MESSAGE msg;
	msg.type   = CLOSE;
	msg.FD     = fd;

	send_recv(BOTH, TASK_FS, &msg);

	return msg.RETVAL;
}