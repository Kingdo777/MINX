global  out_port
global  in_port
;void out_port(uint16_t port,uint8_t value)
out_port:
    mov     dx,[esp+4]
    mov     al,[esp+8]
    out     dx,al
    nop     ;一点延迟
    ret

;uint8_t in_port(uint16_t port)
in_port:
    mov     dx,[esp+4]
    xor     eax,eax
    in      al,dx
    ret