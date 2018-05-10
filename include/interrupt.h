#ifndef _MINX_INTERRUPT_H
#define _MINX_INTERRUPT_H

#define INT_VECTOR_IR0      0x20    //8259A主片起始中断向量号
#define INT_VECTOR_IR8      0x28    //8259A从片起始中断向量号
#define INT_VECTOR_SYS_CALL 0x80    //系统调用起始中断号
#define PORT_8259A_M_CTL    0x20     //8259A主片控制端口号，ICW2的低三位无效，因此中断号要从8的倍数开始
#define PORT_8259A_M_MASK   0x21    //8259A主片屏蔽端口号
#define PORT_8259A_S_CTL    0xA0    //8259A从片控制端口号
#define PORT_8259A_S_MASK   0xA1    //8259A从片屏蔽端口号
#define INT_MASK_NUM_M      0xFF   //主片中断屏蔽码
#define INT_MASK_NUM_S      0xFF    //从片中断屏蔽码

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

void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags);
void    setIdt();
void	set_irq_table(int irq,hwint_handler handler);

#endif