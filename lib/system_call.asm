;给C语言提供的系统调用接口
_NR_get_ticks       equ 0;系统调用功能号
INT_VECTOR_SYS_CALL equ 80h

global  get_ticks

bits    32
section    .text

get_ticks:
    mov     eax,_NR_get_ticks
    int     INT_VECTOR_SYS_CALL
    ret