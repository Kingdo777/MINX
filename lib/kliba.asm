global  out_port
global  in_port
global  enable_irq
global  disable_irq
global  open_hardInt
global  close_hardInt
global  hltf

;void out_port(uint16_t port,uint8_t value)
out_port:
    mov     dx,[esp+4]
    mov     al,[esp+8]
    out     dx,al
    nop     ;一点延迟
    nop
    nop
    nop
    ret

;uint8_t in_port(uint16_t port)
in_port:
    mov     dx,[esp+4]
    xor     eax,eax
    in      al,dx
    nop     ;一点延迟
    nop
    nop
    nop
    ret

; ========================================================================
;                  void disable_irq(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code:
;	if(irq < 8)
;		out_byte(021h, in_byte(021h) | (1 << irq));
;	else
;		out_byte(0A1h, in_byte(0A1h) | (1 << irq));
disable_irq:
        push    ebp
        mov     ebp,esp
        push    eax
        push    ecx
        pushf
        mov     ecx, [ebp+8]          ; irq
        cli
        mov     ah, 1
        rol     ah, cl                  ; ah = (1 << (irq % 8))
        cmp     cl, 8
        jae     disable_8               ; disable irq >= 8 at the slave 8259
disable_0:
        in      al, 021h
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     021h, al       ; set bit at master 8259
        popf
        pop     ecx
        pop     eax
        leave
        ret
disable_8:
        in      al, 0A1h
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     0A1h, al       ; set bit at slave 8259
        popf
        pop     ecx
        pop     eax
        leave
        ret
dis_already:
        popf
        pop     ecx
        pop     eax
        leave
        ret

; ========================================================================
;                  void enable_irq(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;       if(irq < 8)
;               out_byte(021h, in_byte(021h) & ~(1 << irq));
;       else
;               out_byte(0A1h, in_byte(0A1h) & ~(1 << irq));
;
enable_irq:
        push    ebp
        mov     ebp,esp
        push    eax
        push    ecx
        pushf
        mov     ecx, [ebp+8]          ; irq        
        cli
        mov     ah, ~1
        rol     ah, cl                  ; ah = ~(1 << (irq % 8))
        cmp     cl, 8
        jae     enable_8                ; enable irq >= 8 at the slave 8259
enable_0:
        in      al, 021h
        and     al, ah
        out     021h, al       ; clear bit at master 8259
        popf
        pop     ecx
        pop     eax
        leave
        ret
enable_8:
        in      al, 0A1h
        and     al, ah
        out     0A1h, al       ; clear bit at slave 8259
        popf
        pop     ecx
        pop     eax
        leave
        ret
open_hardInt:
        sti
        ret
close_hardInt:
        cli
        ret
hltf:
        hlt