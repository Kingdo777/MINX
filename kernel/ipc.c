#include <stdint.h>
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "string.h"
#include "process.h"
#include "global.h"
#include "ipc.h"
#include "system_call.h"
#include "kliba.h"
#include "stdio.h"
/*****************************************************************************
 *                                send_recv
 *****************************************************************************/
/**
 * <Ring 1~3> IPC syscall.
 *
 * It is an encapsulation of `sendrec',
 * invoking `sendrec' directly should be avoided
 *
 * @param ipc_type  SEND, RECEIVE or BOTH
 * @param dest_process_pid  The caller's proc_nr
 * @param msg       Pointer to the MESSAGE struct
 * 
 * @return always 0.
 *****************************************************************************/
int send_recv(int ipc_type, int dest_process_pid, MESSAGE* msg)
{
	// printf("here1\n");
	int ret = 0;

	if (ipc_type == RECEIVE)
		memset(msg, 0, sizeof(MESSAGE));

	switch (ipc_type) {
	case BOTH:
		ret = sendrec(SEND, dest_process_pid, msg);
		if (ret == 0)
			ret = sendrec(RECEIVE, dest_process_pid, msg);
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(ipc_type, dest_process_pid, msg);
		break;
	default:
		assert((ipc_type == BOTH) ||
		       (ipc_type == SEND) || (ipc_type == RECEIVE));
		break;
	}

	return ret;
}




/*****************************************************************************
 *                                reset_msg
 *****************************************************************************/
/**
 * <Ring 0~3> Clear up a MESSAGE by setting each byte to 0.
 * 
 * @param p  The message to be cleared.
 *****************************************************************************/
void reset_msg(MESSAGE* p)
{
	memset(p, 0, sizeof(MESSAGE));
}

/*****************************************************************************
 *                                block
 *****************************************************************************/
/**
 * <Ring 0> This routine is called after `p_flags' has been set (!= 0), it
 * calls `schedule()' to choose another proc as the `proc_ready'.
 *
 * @attention This routine does not change `p_flags'. Make sure the `p_flags'
 * of the proc to be blocked has been set properly.
 * 
 * @param p The proc to be blocked.
 *****************************************************************************/
void block(PCB* p)
{
	assert(p->p_flags);
	schedule();
}

/*****************************************************************************
 *                                unblock
 *****************************************************************************/
/**
 * <Ring 0> This is a dummy routine. It does nothing actually. When it is
 * called, the `p_flags' should have been cleared (== 0).
 * 
 * @param p The unblocked proc.
 *****************************************************************************/
void unblock(PCB* p)
{
	assert(p->p_flags == 0);
}

/*****************************************************************************
 *                                deadlock
 *****************************************************************************/
/**
 * <Ring 0> Check whether it is safe to send a message from src to dest_process_pid.
 * The routine will detect if the messaging graph contains a cycle. For
 * instance, if we have procs trying to send messages like this:
 * A -> B -> C -> A, then a deadlock occurs, because all of them will
 * wait forever. If no cycles detected, it is considered as safe.
 * 
 * @param src   Who wants to send message.
 * @param dest_process_pid  To whom the message is sent.
 * 
 * @return Zero if success.
 *****************************************************************************/
int deadlock(int src, int dest_process_pid)
{
	PCB* p = pcb_table + dest_process_pid;
	while (1) {
		if (p->p_flags & SENDING) {
			if (p->p_sendto == src) {
				/* print the chain */
				p = pcb_table + dest_process_pid;
				printl("=_=%d", p->pid);
				do {
					assert(p->p_msg);
					p = pcb_table + p->p_sendto;
					printl("->%d", p->pid);
				} while (p != pcb_table + src);
				printl("=_=");

				return 1;
			}
			p = pcb_table + p->p_sendto;
		}
		else {
			break;
		}
	}
	return 0;
}

/*****************************************************************************
 *                                msg_send
 *****************************************************************************/
/**
 * <Ring 0> Send a message to the dest_process_pid proc. If dest_process_pid is blocked waiting for
 * the message, copy the message to it and unblock dest_process_pid. Otherwise the caller
 * will be blocked and appended to the dest_process_pid's sending queue.
 * 
 * @param sender  The caller, the sender.
 * @param dest_process_pid     To whom the message is sent.
 * @param m        The message.
 * 
 * @return Zero if success.
 *****************************************************************************/
int msg_send(PCB* sender, int dest_process_pid, MESSAGE* m)
{
	PCB* receiver = pcb_table + dest_process_pid; /* proc dest_process_pid */

	assert(getpid(sender) != dest_process_pid);

	/* check for deadlock here */
	if (deadlock(getpid(sender), dest_process_pid)) {
		panic(">>DEADLOCK<< %d->%d", sender->pid, receiver->pid);
	}

	if ((receiver->p_flags & RECEIVING) && /* dest_process_pid is waiting for the msg */
	    (receiver->p_recvfrom == getpid(sender) ||
	     receiver->p_recvfrom == ANY)) {
		assert(receiver->p_msg);
		assert(m);

		// phys_copy(va2la(dest_process_pid, receiver->p_msg),
		// 	  va2la(getpid(sender), m),
		// 	  sizeof(MESSAGE));
		*(receiver->p_msg)=*(m);
		receiver->p_msg = 0;
		receiver->p_flags &= ~RECEIVING; /* dest_process_pid has received the msg */
		receiver->p_recvfrom = NO_TASK;
		unblock(receiver);

		assert(receiver->p_flags == 0);
		assert(receiver->p_msg == 0);
		assert(receiver->p_recvfrom == NO_TASK);
		assert(receiver->p_sendto == NO_TASK);
		assert(sender->p_flags == 0);
		assert(sender->p_msg == 0);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == NO_TASK);
	}
	else { /* dest_process_pid is not waiting for the msg */
		sender->p_flags |= SENDING;
		assert(sender->p_flags == SENDING);
		sender->p_sendto = dest_process_pid;
		sender->p_msg = m;

		/* append to the sending queue */
		PCB * p;
		if (receiver->q_sending) {
			p = receiver->q_sending;
			while (p->next_sending)
				p = p->next_sending;
			p->next_sending = sender;
		}
		else {
			receiver->q_sending = sender;
		}
		sender->next_sending = 0;

		block(sender);

		assert(sender->p_flags == SENDING);
		assert(sender->p_msg != 0);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == dest_process_pid);
	}

	return 0;
}


/*****************************************************************************
 *                                msg_receive
 *****************************************************************************/
/**
 * <Ring 0> Try to get a message from the sender_pid proc. If sender_pid is blocked sending
 * the message, copy the message from it and unblock sender_pid. Otherwise the caller
 * will be blocked.
 * 
 * @param receiver The caller, the proc who wanna receive.
 * @param sender_pid     From whom the message will be received.
 * @param m       The message ptr to accept the message.
 * 
 * @return  Zero if success.
 *****************************************************************************/
int msg_receive(PCB* receiver, int sender_pid, MESSAGE* m)
{
	// printf("here1\n");	
	PCB* sender = 0; /* from which the message will be fetched */
	PCB* prev = 0;
	int copyok = 0;

	assert(getpid(receiver) != sender_pid);

	if ((receiver->has_int_msg) &&
	    ((sender_pid == ANY) || (sender_pid == INTERRUPT))) {
		/* There is an interrupt needs receiver's handling and
		 * receiver is ready to handle it.
		 */

		MESSAGE msg;
		reset_msg(&msg);
		msg.source = INTERRUPT;
		msg.type = HARD_INT;

		assert(m);

		// phys_copy(va2la(getpid(receiver), m), &msg,
		// 	  sizeof(MESSAGE));
		*(m)=msg;
		receiver->has_int_msg = 0;

		assert(receiver->p_flags == 0);
		assert(receiver->p_msg == 0);
		assert(receiver->p_sendto == NO_TASK);
		assert(receiver->has_int_msg == 0);

		return 0;
	}

	/* Arrives here if no interrupt for receiver. */
	if (sender_pid == ANY) {
		/* receiver is ready to receive messages from
		 * ANY proc, we'll check the sending queue and pick the
		 * first proc in it.
		 */
		if (receiver->q_sending) {
			sender = receiver->q_sending;
			copyok = 1;
			// printf("here1\n");
			assert(receiver->p_flags == 0);
			assert(receiver->p_msg == 0);
			assert(receiver->p_recvfrom == NO_TASK);
			assert(receiver->p_sendto == NO_TASK);
			assert(receiver->q_sending != 0);
			assert(sender->p_flags == SENDING);
			assert(sender->p_msg != 0);
			assert(sender->p_recvfrom == NO_TASK);
			assert(sender->p_sendto == getpid(receiver));
		}
	}
	else if (sender_pid >= 0 && sender_pid < NR_TASK + NR_USER_PROCESS) {
		/* receiver wants to receive a message from
		 * a certain proc: sender_pid.
		 */
		sender = &pcb_table[sender_pid];

		if ((sender->p_flags & SENDING) &&
		    (sender->p_sendto == getpid(receiver))) {
			/* Perfect, sender_pid is sending a message to
			 * receiver.
			 */
			copyok = 1;

			PCB* p = receiver->q_sending;

			assert(p); /* sender must have been appended to the
				    * queue, so the queue must not be NULL
				    */

			while (p) {
				assert(sender->p_flags & SENDING);

				if (getpid(p) == sender_pid) /* if p is the one */
					break;

				prev = p;
				p = p->next_sending;
			}

			assert(receiver->p_flags == 0);
			assert(receiver->p_msg == 0);
			assert(receiver->p_recvfrom == NO_TASK);
			assert(receiver->p_sendto == NO_TASK);
			assert(receiver->q_sending != 0);
			assert(sender->p_flags == SENDING);
			assert(sender->p_msg != 0);
			assert(sender->p_recvfrom == NO_TASK);
			assert(sender->p_sendto == getpid(receiver));
		}
	}

	if (copyok) {
		/* It's determined from which proc the message will
		 * be copied. Note that this proc must have been
		 * waiting for this moment in the queue, so we should
		 * remove it from the queue.
		 */
		if (sender == receiver->q_sending) { /* the 1st one */
			assert(prev == 0);
			receiver->q_sending = sender->next_sending;
			sender->next_sending = 0;
		}
		else {
			assert(prev);
			prev->next_sending = sender->next_sending;
			sender->next_sending = 0;
		}

		assert(m);
		assert(sender->p_msg);

		/* copy the message */
		// phys_copy(va2la(getpid(receiver), m),
		// 	  va2la(getpid(sender), sender->p_msg),
		// 	  sizeof(MESSAGE));
		*(m)=*(sender->p_msg);
		sender->p_msg = 0;
		sender->p_sendto = NO_TASK;
		sender->p_flags &= ~SENDING;

		unblock(sender);
	}
	else {  /* nobody's sending any msg */
		/* Set p_flags so that receiver will not
		 * be scheduled until it is unblocked.
		 */
		receiver->p_flags |= RECEIVING;

		receiver->p_msg = m;
		receiver->p_recvfrom = sender_pid;
		block(receiver);

		assert(receiver->p_flags == RECEIVING);
		assert(receiver->p_msg != 0);
		assert(receiver->p_recvfrom != NO_TASK);
		assert(receiver->p_sendto == NO_TASK);
		assert(receiver->has_int_msg == 0);
	}

	return 0;
}

/*****************************************************************************
 *                                getpid
 *****************************************************************************/
int getpid(PCB *p)
{
    return p->pid;
}
/*****************************************************************************
 *                                inform_int
 *****************************************************************************/
void inform_int(int task_nr)
{
	struct proc* p = pcb_table + task_nr;

	if ((p->p_flags & RECEIVING) && /* dest is waiting for the msg */
	    ((p->p_recvfrom == INTERRUPT) || (p->p_recvfrom == ANY))) {
		p->p_msg->source = INTERRUPT;
		p->p_msg->type = HARD_INT;
		p->p_msg = 0;
		p->has_int_msg = 0;
		p->p_flags &= ~RECEIVING; /* dest has received the msg */
		p->p_recvfrom = NO_TASK;
		assert(p->p_flags == 0);
		unblock(p);

		assert(p->p_flags == 0);
		assert(p->p_msg == 0);
		assert(p->p_recvfrom == NO_TASK);
		assert(p->p_sendto == NO_TASK);
	}
	else {
		p->has_int_msg = 1;
	}
}

/*****************************************************************************
 *                                dump_proc
 *****************************************************************************/
void dump_proc(PCB* p)
{
	set_out_char_highLight(MAKE_COLOR(GREEN,RED));
	char info[1024];
	int i;

	int dump_len = sizeof(PCB);

	printf("byte dump of pcb_table[%d]:\n", p - pcb_table); 
	for (i = 0; i < dump_len; i++) {
		printf("%d.", ((unsigned char *)p)[i]);
		
	}

	NL();
	printf("ANY: %d.\n", ANY); 
	printf("NO_TASK: %d.\n", NO_TASK); 
	NL();

	printf("ldt_sel: %d.  \n", p->ldt_sel); 
	printf("ticks: %d.  \n", p->ticks); 
	printf("priority: %d.  \n", p->priority); 
	printf("pid: %d.  \n", p->pid); 
	NL();
	printf("p_flags: %d.  \n", p->p_flags); 
	printf("p_recvfrom: %d.  \n", p->p_recvfrom); 
	printf("p_sendto: %d.  \n", p->p_sendto); 
	printf("nr_tty: %d.  \n", p->tty-tty_table); 
	NL();
	printf("has_int_msg: %d.  \n", p->has_int_msg); 
	set_out_char_highLight(MAKE_COLOR(BLACK,WHITE));	
}


/*****************************************************************************
 *                                dump_msg
 *****************************************************************************/
void dump_msg(const char * title, MESSAGE* m)
{
	int packed = 0;
	printl("{%s}<%d>{%spid:%d,%stype:%d,%s(%d, %d, %d, %d, %d, %d)%s}%s",  //, (%d, %d, %d)}",
	       title,
	       (int)m,
	       packed ? "" : "\n        ",
	       m->source,
	       packed ? " " : "\n        ",
	       m->type,
	       packed ? " " : "\n        ",
	       m->u.m3.m3i1,
	       m->u.m3.m3i2,
	       m->u.m3.m3i3,
	       m->u.m3.m3i4,
	       (int)m->u.m3.m3p1,
	       (int)m->u.m3.m3p2,
	       packed ? "" : "\n",
	       packed ? "" : "\n"/* , */
		);
}
