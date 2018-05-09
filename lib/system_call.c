#include "const.h"
#include "global.h"
//此函数用于实现system_call_table中的函数
int sys_get_ticks(){
    return ticksCount;
}