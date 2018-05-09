#ifndef _MINX_CONST_H
#define _MINX_CONST_H
#include <stdint.h>

#define GDT_SIZE 128
#define LDT_SIZE 1
#define IDT_SIZE 256
#define TASK_STACK_SIZE 4 * 1024
#define NR_TASK 2
#define NR_IRQ  16

#define INT_VECTOR_IR0      0x20    //8259A主片起始中断向量号
#define INT_VECTOR_IR8      0x28    //8259A从片起始中断向量号
#define PORT_8259A_M_CTL    0x20     //8259A主片控制端口号，ICW2的低三位无效，因此中断号要从8的倍数开始
#define PORT_8259A_M_MASK   0x21    //8259A主片屏蔽端口号
#define PORT_8259A_S_CTL    0xA0    //8259A从片控制端口号
#define PORT_8259A_S_MASK   0xA1    //8259A从片屏蔽端口号
#define INT_MASK_NUM_M      0xFF   //主片中断屏蔽码
#define INT_MASK_NUM_S      0xFF    //从片中断屏蔽码

typedef void  (*hwint_handler)(int irq);//硬件中断处理函数数组

typedef struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t attr_low;  //TYPE(4),S(1),DPL(2),P(1)
    uint8_t attr_high; //limit_hight(4),0(1),D/B(1),G(1)
    uint8_t base_high;
} DESCRIPTOR;

typedef struct
{ //此结构体的设计中将gdt_base划分为了两个16字节主要是考虑了字节对其，或者说是避免字节对其的影响
    uint16_t gdt_limit;
    uint16_t gdt_base;
    uint16_t gdt_base_high;
} GDT_PTR;

typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t dcount; /*仅仅在调用门中有意义，在发生任务切换时，若特权级发生变化将引起堆栈的切换，此时需要复制外层堆栈到内层堆栈，dcount指出需要复制的参数的个数*/
    uint8_t attr;   //P(1),DPL(2),DT(1),TYPE(4)
    uint16_t offset_high;
} GATE;

typedef struct
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t kernel_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t retaddr;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} STACK_FRAME;
typedef struct
{
    STACK_FRAME regs;

    uint16_t ldt_sel;
    DESCRIPTOR ldt[LDT_SIZE];

    uint32_t pid;
    char pName[16];
} PCB;

typedef struct{
    uint32_t eip;
    uint32_t topOfStack;
}TASK;

typedef struct
{
    uint32_t backlink;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iobase;

} TSS;

#define HEX 16
#define DEC 10

/* GDT */
/* 描述符索引 */
#define INDEX_DUMMY 0
#define INDEX_CORE_DATA_4G 1
#define INDEX_CORE_CODE_4G 2
#define INDEX_TASK_DATA_4G 3
#define INDEX_TASK_CODE_4G 4
#define INDEX_LDT 5
#define INDEX_TSS 6

/* 选择子 */
#define SELECTOR_DUMMY INDEX_DUMMY * 8
#define SELECTOR_CORE_DATA_4G INDEX_CORE_DATA_4G * 8
#define SELECTOR_CORE_CODE_4G INDEX_CORE_CODE_4G * 8
#define SELECTOR_TASK_DATA_4G INDEX_TASK_DATA_4G * 8 + PRIVILEGE_TASK
#define SELECTOR_TASK_CODE_4G INDEX_TASK_CODE_4G * 8 + PRIVILEGE_TASK
#define SELECTOR_LDT INDEX_LDT * 8
#define SELECTOR_TSS INDEX_TSS * 8

/* 描述符类型值说明 */
#define DA_32 0x4000       /* 32 位段				*/
#define DA_LIMIT_4K 0x8000 /* 段界限粒度为 4K 字节			*/
#define DA_DPL0 0x00       /* DPL = 0				*/
#define DA_DPL1 0x20       /* DPL = 1				*/
#define DA_DPL2 0x40       /* DPL = 2				*/
#define DA_DPL3 0x60       /* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define DA_DR 0x90   /* 存在的只读数据段类型值		*/
#define DA_DRW 0x92  /* 存在的可读写数据段属性值		*/
#define DA_DRWA 0x93 /* 存在的已访问可读写数据段类型值	*/
#define DA_C 0x98    /* 存在的只执行代码段属性值		*/
#define DA_CR 0x9A   /* 存在的可执行可读代码段属性值		*/
#define DA_CCO 0x9C  /* 存在的只执行一致代码段属性值		*/
#define DA_CCOR 0x9E /* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define DA_LDT 0x82      /* 局部描述符表段类型值			*/
#define DA_TaskGate 0x85 /* 任务门类型值				*/
#define DA_386TSS 0x89   /* 可用 386 任务状态段类型值		*/
#define DA_386CGate 0x8C /* 386 调用门类型值			*/
#define DA_386IGate 0x8E /* 386 中断门类型值			*/
#define DA_386TGate 0x8F /* 386 陷阱门类型值			*/

#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3

/* 中断向量 */
#define INT_VECTOR_DIVIDE 0x0
#define INT_VECTOR_DEBUG 0x1
#define INT_VECTOR_NMI 0x2
#define INT_VECTOR_BREAKPOINT 0x3
#define INT_VECTOR_OVERFLOW 0x4
#define INT_VECTOR_BOUNDS 0x5
#define INT_VECTOR_INVAL_OP 0x6
#define INT_VECTOR_COPROC_NOT 0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG 0x9
#define INT_VECTOR_INVAL_TSS 0xA
#define INT_VECTOR_SEG_NOT 0xB
#define INT_VECTOR_STACK_FAULT 0xC
#define INT_VECTOR_PROTECTION 0xD
#define INT_VECTOR_PAGE_FAULT 0xE
#define INT_VECTOR_COPROC_ERR 0x10

#endif