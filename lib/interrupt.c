#include"const.h"
#include"kliba.h"
#include"mystring.h"

extern GDT_PTR idt_ptr;
extern GATE idt[IDT_SIZE];


/* 中断处理函数 */
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

void init_8259A(){
//设置主从片ICW1
	out_port(PORT_8259A_M_CTL,0x11);
	out_port(PORT_8259A_S_CTL,0x11);
//设置主从片ICW2
	out_port(PORT_8259A_M_MASK,INT_VECTOR_IR0);
	out_port(PORT_8259A_S_MASK,INT_VECTOR_IR8);
//设置主从片ICW3
	out_port(PORT_8259A_M_MASK,0x04);
	out_port(PORT_8259A_S_MASK,0x02);
//设置主从片ICW4
	out_port(PORT_8259A_M_MASK,0x01);
	out_port(PORT_8259A_S_MASK,0x01);
//设置OCW1
	out_port(PORT_8259A_M_MASK,0xFD);
	out_port(PORT_8259A_S_MASK,0xFF);
}

/*======================================================================*
 init_idt_desc
 *----------------------------------------------------------------------*
 初始化 386 中断门
 *======================================================================*/
void  init_idt_desc(uint8_t vector, uint8_t desc_type,void * handler, uint8_t privilege)
{
	GATE *p_gate= &idt[vector];
	uint32_t base= (uint32_t)handler;
	p_gate->offset_low= base & 0xFFFF;
	p_gate->selector= selector_CORE_CODE_4G;
	p_gate->dcount= 0;
	p_gate->attr= desc_type | (privilege << 5);
	p_gate->offset_high= (base >> 16) & 0xFFFF;
}

void setIdt(){
//初始化8259A
	init_8259A();
//转移IDT
	idt_ptr.gdt_limit=(uint16_t)(IDT_SIZE*sizeof(GATE)-1);
	*((uint32_t*)(&idt_ptr.gdt_base))=(uint32_t)(idt);
// 填充IDT的异常处理（全部处理为中断门）
	init_idt_desc(INT_VECTOR_DIVIDE,DA_386IGate,divide_error,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DEBUG,DA_386IGate,single_step_exception,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_NMI,DA_386IGate,nmi,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_BREAKPOINT,DA_386IGate,breakpoint_exception,PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_OVERFLOW,DA_386IGate,overflow,PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_BOUNDS,DA_386IGate,bounds_check,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_OP,DA_386IGate,inval_opcode,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_NOT,DA_386IGate,copr_not_available,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,DA_386IGate,double_fault,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_SEG,DA_386IGate,copr_seg_overrun,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_TSS,DA_386IGate,inval_tss,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_SEG_NOT,DA_386IGate,segment_not_present,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_STACK_FAULT,DA_386IGate,stack_exception,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PROTECTION,DA_386IGate,general_protection,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PAGE_FAULT,DA_386IGate,page_fault,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_ERR,DA_386IGate,copr_error,PRIVILEGE_KRNL);
//填充IDT的硬件中断处理
	init_idt_desc(INT_VECTOR_IR0 + 0,DA_386IGate,hwint00,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 1,DA_386IGate,hwint01,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 2,DA_386IGate,hwint02,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 3,DA_386IGate,hwint03,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 4,DA_386IGate,hwint04,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 5,DA_386IGate,hwint05,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 6,DA_386IGate,hwint06,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR0 + 7,DA_386IGate,hwint07,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 0,DA_386IGate,hwint08,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 1,DA_386IGate,hwint09,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 2,DA_386IGate,hwint10,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 3,DA_386IGate,hwint11,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 4,DA_386IGate,hwint12,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 5,DA_386IGate,hwint13,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 6,DA_386IGate,hwint14,PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IR8 + 7,DA_386IGate,hwint15,PRIVILEGE_KRNL);
}


/*======================================================================*
exception_handler
 *----------------------------------------------------------------------*
 异常处理
 *======================================================================*/
void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags)
{
	char err_msg[20][50] = {
		"#DE Divide Error",
		"#DB RESERVED",
		"--NMI Interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR BOUND Range Exceeded",
		"#UD Invalid Opcode (Undefined Opcode)",
		"#NM Device Not Available (No Math Coprocessor)",
		"#DF Double Fault",
		"Coprocessor Segment Overrun (reserved)",
		"#TS Invalid TSS",
		"#NP Segment Not Present",
		"#SS Stack-Segment Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"--(Intel reserved. Do not use.)",
		"#MF x87 FPU Floating-Point Error (Math Fault)",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD Floating-Point Exception"
	};
	puts(0x04,"Exception! --> ");
	puts(0x04,err_msg[vec_no]);
}
void hardWareInt_handler(int irq)
{
	char	s[32];
    puts(0x04,"spurious_irq: ");
    puts(0x04,itoa(irq,s,HEX));
    puts(0x04,"\n");
}
void	breakPointDebug(int *esp,int count){
	char	s[32];
	for(int i=0;i<count;i++){
		puts(0x04,itoa(*esp++,s,HEX));
		puts(0x04,"\n");
	}
}