#include "const.h"
#include "protect.h"
#include "process.h"
#include "system_call.h"
#include "tty.h"
#include "fs.h"
#include <stdint.h>

void TestA();
void TestB();
void TestC();
void task_tty();
void task_sys();
void task_hd();
void task_fs();
void Init();
void task_mm();

void test_in_asmA();
void test_in_asmB();

GDT_PTR gdt_ptr;
DESCRIPTOR gdt[GDT_SIZE];

GDT_PTR idt_ptr;
GATE idt[IDT_SIZE];

PCB pcb_table[NR_TASK + NR_USER_PROCESS];
PCB *pcb_ptr;

TSS tss;

char task_stack_A[TASK_STACK_SIZE];
char task_stack_B[TASK_STACK_SIZE];
char task_stack_C[TASK_STACK_SIZE];
char task_stack_Init[TASK_STACK_SIZE];
char task_stack_tty[TASK_STACK_SIZE];
char task_stack_sys[TASK_STACK_SIZE];
char task_stack_hd[TASK_STACK_SIZE];
char task_stack_fs[TASK_STACK_SIZE];
char task_stack_mm[TASK_STACK_SIZE];

uint32_t Int_reEnter;

TASK task[NR_TASK] = 
{
    {(uint32_t)task_tty, (uint32_t)task_stack_tty + TASK_STACK_SIZE,"TTY"},
    {(uint32_t)task_sys, (uint32_t)task_stack_sys + TASK_STACK_SIZE,"SYS"},
    {(uint32_t)task_hd, (uint32_t)task_stack_hd + TASK_STACK_SIZE,"HD"},
    {(uint32_t)task_fs, (uint32_t)task_stack_fs + TASK_STACK_SIZE,"FS"},
    {(uint32_t)task_mm, (uint32_t)task_stack_mm + TASK_STACK_SIZE,"MM"}
};

TASK user_process[NR_USER_PROCESS]=
{
  {(uint32_t)TestA, (uint32_t)task_stack_A + TASK_STACK_SIZE,"TestA"},
  {(uint32_t)TestB, (uint32_t)task_stack_B + TASK_STACK_SIZE,"TestB"},
  {(uint32_t) TestC, (uint32_t)task_stack_C + TASK_STACK_SIZE,"TestC"},
  {(uint32_t) Init, (uint32_t)task_stack_Init + TASK_STACK_SIZE,"Init"}
};

hwint_handler irq_table[NR_IRQ];

system_call_var system_call_func_table[NR_SYS_CALL] = {
    sys_get_ticks,
    // sys_write,
    sys_printx,
    sys_sendrec
};

int ticksCount; //时钟中断次数

TTY tty_table[NR_TTY];
CONSOLE console_table[NR_TTY];
TTY *current_tty;

uint8_t out_char_highLight=0x07;

/* FS related below */
/*****************************************************************************/
/**
 * For dd_map[k],
 * `k' is the device nr.\ dd_map[k].driver_nr is the driver nr.
 *
 * Remeber to modify include/const.h if the order is changed.
 *****************************************************************************/
struct dev_drv_map dd_map[] = {
	/* driver nr.		major device nr.
	   ----------		---------------- */
	{INVALID_DRIVER},	/**< 0 : Unused */
	{INVALID_DRIVER},	/**< 1 : Reserved for floppy driver */
	{INVALID_DRIVER},	/**< 2 : Reserved for cdrom driver */
	{TASK_HD},		/**< 3 : Hard disk */
	{TASK_TTY},		/**< 4 : TTY */
	{INVALID_DRIVER}	/**< 5 : Reserved for scsi disk driver */
};

/**
 * 6MB~7MB: buffer for FS
 */
uint8_t *	fsbuf	= (uint8_t*)0x600000;
const int	FSBUF_SIZE	= 0x100000;

struct file_desc	f_desc_table[NR_FILE_DESC];
struct inode		inode_table[NR_INODE];
struct super_block	super_block[NR_SUPER_BLOCK];
MESSAGE			fs_msg;
PCB *pcaller;
struct inode *	root_inode;
int	key_pressed=0;