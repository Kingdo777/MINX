;无参数，关闭软驱马达
KillMotor:
    push    dx
    push    ax
    mov     dx,03f2h
    mov     al,0
    out     dx,al
    pop     ax
    pop     dx
    ret
