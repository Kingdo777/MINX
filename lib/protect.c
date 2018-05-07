#include    "const.h"
#include    "protect.h"
#include    "mystring.h"
#include    "global.h"
#include    <stdint.h>


void resetGdt(DESCRIPTOR gdt[],GDT_PTR *gdt_ptr){
    memcpy((void *)gdt,(void *)(*((uint32_t *)(&gdt_ptr->gdt_base))),(uint32_t)(gdt_ptr->gdt_limit)+1);
    *((uint32_t*)(&gdt_ptr->gdt_base))=(uint32_t)(gdt);
}

void init_descriptor(DESCRIPTOR *des,uint32_t base,uint32_t limit,uint16_t attr){
    des->base_high  =   base>>24;
    des->base_mid   =  (base>>16)&0xff;
    des->base_low   =   base&0xffff;
    des->limit_low  =   limit&0xffff;
    des->attr_high  =   ((limit>>16)&0x0f)|((attr>>8)&0xf0);
    des->attr_low   =   attr&0xff;
}

void setTss(){
    memset((void *)&tss,0,sizeof(TSS));
    tss.ss0=SELECTOR_CORE_CODE_4G;//因为我们这里仅仅设计ring0和ring3之间的特权级转换，因此仅仅设置ss0即可
    tss.iobase=sizeof(TSS)-1;//不设置I/O许可位图  
      
    init_descriptor(&gdt[INDEX_TSS],(uint32_t)&tss,sizeof(TSS)-1,DA_386TSS|DA_DPL1);
}
// uint32_t segToPhys(uint16_t seg){
//     DESCRIPTOR  *desc=&gdt[seg>>3];
//     return  (desc->base_high<<24|desc->base_mid<<16|base_low);
// }
