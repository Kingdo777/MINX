#ifndef _MINX_PROTECT_H
#define _MINX_PROTECT_H

#include    "const.h"

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

/* GDT */
/* 描述符索引 */
#define INDEX_DUMMY 0
#define INDEX_CORE_DATA_4G 1
#define INDEX_CORE_CODE_4G 2
#define INDEX_TASK_DATA_4G 3
#define INDEX_TASK_CODE_4G 4
#define INDEX_USER_DATA_4G 5
#define INDEX_USER_CODE_4G 6
#define INDEX_LDT 7
#define INDEX_TSS 8

/* 选择子 */
#define SELECTOR_DUMMY INDEX_DUMMY * 8
#define SELECTOR_CORE_DATA_4G INDEX_CORE_DATA_4G * 8
#define SELECTOR_CORE_CODE_4G INDEX_CORE_CODE_4G * 8
#define SELECTOR_TASK_DATA_4G INDEX_TASK_DATA_4G * 8 + PRIVILEGE_TASK
#define SELECTOR_TASK_CODE_4G INDEX_TASK_CODE_4G * 8 + PRIVILEGE_TASK
#define SELECTOR_USER_DATA_4G INDEX_USER_DATA_4G * 8 + PRIVILEGE_USER
#define SELECTOR_USER_CODE_4G INDEX_USER_CODE_4G * 8 + PRIVILEGE_USER
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

void resetGdt(DESCRIPTOR gdt[],GDT_PTR *gdt_ptr);
void init_descriptor(DESCRIPTOR *des,uint32_t base,uint32_t limit,uint16_t attr);
void setTss();
#endif