;给C语言提供的系统调用接口
_NR_get_ticks       equ 0;系统调用功能号
_NR_write           equ 1;系统调用功能号
_NR_printx          equ 2
_NR_sendrec         equ 3
INT_VECTOR_SYS_CALL equ 80h

global  get_ticks
global  write
global  printx
global  sendrec

bits    32
section    .text

get_ticks:
    mov     eax,_NR_get_ticks
    int     INT_VECTOR_SYS_CALL
    ret
write:
    push    ebp
    mov     ebp,esp
    push    edx;eax是返回值，万不可保存
    push    ecx
    mov     eax,_NR_write
    mov     ecx,[ebp+8];char *buf
    mov     edx,[ebp+12];int len
    int     INT_VECTOR_SYS_CALL
    pop     ecx
    pop     edx
    leave
    ret
printx:
    push    ebp
    mov     ebp,esp
    push    edx;eax是返回值，万不可保存
    mov     eax,_NR_printx
    mov     edx,[ebp+8];char *buf
    int     INT_VECTOR_SYS_CALL
    pop     edx
    leave
    ret
sendrec:
    push    ebp
    mov     ebp,esp
    push    edx;eax是返回值，万不可保存
    push    ecx
    push    ebx
    mov     eax,_NR_sendrec
    mov     ebx,[ebp+8];int ipc_type
    mov     ecx,[ebp+12];int dest_task_pid
    mov     edx,[ebp+16];Message *m
    int     INT_VECTOR_SYS_CALL
    pop     ebx
    pop     ecx
    pop     edx
    leave
    ret
