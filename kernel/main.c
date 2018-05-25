#include "kliba.h"
#include "string.h"
#include "const.h"
#include "global.h"
#include "interrupt.h"
#include "system_call.h"
#include "clock.h"
#include "keyboard.h"
#include "ipc.h"
#include "stdio.h"

void restart();
void test_in_asm();

void kernelMain()
{
    PCB *p;
    char tmp[256]="\0";
    for (int i = 0; i < NR_TASK+NR_USER_PROCESS; i++)
    {
        p = &pcb_table[i];
        p->ldt_sel = SELECTOR_LDT;
        p->pid = i;
        p->ticks = pcb_table[i].priority = 30; //为每个进程初始化权值       
        p->tty = tty_table; //为每个进程初始tty终端  
        //消息相关
        p->p_flags = 0;
		p->p_msg = 0;
		p->p_recvfrom = NO_TASK;
		p->p_sendto = NO_TASK;
		p->has_int_msg = 0;
		p->q_sending = 0;
		p->next_sending = 0;
        sprintf(tmp,"process%d",i); 
        p->name=tmp;   
        if(i<NR_TASK){
            //是TASK
            p->regs.cs = SELECTOR_TASK_CODE_4G;
            p->regs.ds = SELECTOR_TASK_DATA_4G;
            p->regs.es = SELECTOR_TASK_DATA_4G;
            p->regs.gs = SELECTOR_TASK_DATA_4G;
            p->regs.fs = SELECTOR_TASK_DATA_4G;
            p->regs.ss = SELECTOR_TASK_DATA_4G;
            p->regs.eip = (uint32_t)task[i].eip;
            p->regs.esp = (uint32_t)task[i].topOfStack;
            p->regs.eflags = 0x1202; //IF=1,IOPL=1,第二位恒为1
        }else{
            p->regs.cs = SELECTOR_USER_CODE_4G;
            p->regs.ds = SELECTOR_USER_DATA_4G;
            p->regs.es = SELECTOR_USER_DATA_4G;
            p->regs.gs = SELECTOR_USER_DATA_4G;
            p->regs.fs = SELECTOR_USER_DATA_4G;
            p->regs.ss = SELECTOR_USER_DATA_4G;
            p->regs.eip = (uint32_t)user_process[i-NR_TASK].eip;
            p->regs.esp = (uint32_t)user_process[i-NR_TASK].topOfStack;
            p->regs.eflags = 0x0202; //IF=1,IOPL=0,第二位恒为1
        } 
    }
    //为每个进程初始化权值
    // pcb_table[0].ticks=pcb_table[0].priority=30;
    // pcb_table[1].ticks=pcb_table[1].priority=30;
    // pcb_table[2].ticks=pcb_table[2].priority=30;
    // pcb_table[3].ticks=pcb_table[2].priority=30;

    pcb_ptr = pcb_table;
    Int_reEnter = 0; //中断重入检测
    ticksCount = 0;  //很奇怪，我在global.c中对ticksCount的初始化无效

    init_all_tty();//初始化TTY
    init_clock();

    restart();
    while (1)
        ;
}

void TestA()
{
    int fd;
	int i, n;

	char filename[MAX_FILENAME_LEN+1] = "blah";
	const char bufw[] = "abcde";
	const int rd_bytes = 3;
	char bufr[rd_bytes];

	assert(rd_bytes <= strlen(bufw));

	/* create */
	fd = open(filename, O_CREAT | O_RDWR);
	assert(fd != -1);
	printf("File created: %s (fd %d)\n", filename, fd);

	/* write */
	n = write(fd, bufw, strlen(bufw));
	assert(n == strlen(bufw));

	/* close */
	close(fd);

	/* open */
	fd = open(filename, O_RDWR);
	assert(fd != -1);
	printf("File opened. fd: %d\n", fd);

	/* read */
	n = read(fd, bufr, rd_bytes);
	assert(n == rd_bytes);
	bufr[n] = 0;
	printf("%d bytes read: %s\n", n, bufr);

	/* close */
	close(fd);

	char * filenames[] = {"/foo", "/bar", "/baz"};

	/* create files */
	for (i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++) {
		fd = open(filenames[i], O_CREAT | O_RDWR);
		assert(fd != -1);
		printf("File created: %s (fd %d)\n", filenames[i], fd);
		close(fd);
	}

	char * rfilenames[] = {"/bar", "/foo", "/baz", "/dev_tty0"};

	/* remove files */
	for (i = 0; i < sizeof(rfilenames) / sizeof(rfilenames[0]); i++) {
		if (unlink(rfilenames[i]) == 0)
			printf("File removed: %s\n", rfilenames[i]);
		else
			printf("Failed to remove file: %s\n", rfilenames[i]);
	}

	spin("TestA");

    
    // dump_proc(pcb_ptr);
    // printf("###%c%c%c###\n",'1','2','3');
    // printf("current_start_addr:%d\n",current_tty->p_console->current_start_addr);
    // printf("original_addr:%d\n",current_tty->p_console->original_addr);
    // printf("v_mem_limit:%d\n",current_tty->p_console->v_mem_limit);
    // printf("cursor:(%d,%d)\n",current_tty->p_console->cursor/80,current_tty->p_console->cursor%80);
    // int i=0;
    // while (i++<100)
    // {
        // get_ticks_by_message();
        // putNum(get_ticks_by_message(),10);
        // printf("---");
        // putNum(get_ticks(),10);

        // set_out_char_highLight(4);               
        // puts("A.");
        // delay(10);
    // }
    // while(1);
}

void TestB()
{
    char tty_name[] = "/dev_tty1";

	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];

	while (1) {
		write(fd_stdout, "$ ", 2);
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;

		if (strcmp(rdbuf, "hello") == 0) {
			write(fd_stdout, "hello world!\n", 13);
		}
		else {
			if (rdbuf[0]) {
				write(fd_stdout, "{", 1);
				write(fd_stdout, rdbuf, r);
				write(fd_stdout, "}\n", 2);
			}
		}
	}

	assert(0); /* never arrive here */
    // int i=0;    
    // panic("TestB()");
    // assert(0);
    // while (i++<100)
    // {
        // set_out_char_highLight(5);        
        // puts("B.");
        // delay(10);
    // }
    // while(1);    
}

void TestC()
{
    int i=0;
    while (i++<100)
    {
        set_out_char_highLight(6);        
        // puts("C.");
        // delay(10);
    }
    while(1);    
}
void task_sys(){
    MESSAGE msg;
	while (1) {
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		switch (msg.type) {
		case GET_TICKS:
			msg.RETVAL = ticksCount;
			send_recv(SEND, src, &msg);
			break;
        case GET_PID:
			msg.type = SYSCALL_RET;
			msg.PID = src;
			send_recv(SEND, src, &msg);
			break;
		default:
			panic("unknown msg type");
			break;
		}
	}
}