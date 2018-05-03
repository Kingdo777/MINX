graphicsMemoryPosition	equ	0xB8000

global  memcpy      ;void memcpy(void*dest,void*src,uint32_t size);
global  puts        ;void puts(uint8_t attr,char *s);
global  putchar     ;void putchar(uint8_t attr,char s);

;void memcpy(void*dest,void*src,uint32_t size);
memcpy:
    push    ebp
    mov     ebp,esp
    push    ecx
    push    esi
    push    edi
    mov     ecx,[ebp+16]
    mov     esi,[ebp+12]
    mov     edi,[ebp+8]
    rep     movsb
    pop     eax
    pop     edi
    pop     esi
    pop     ecx
    leave
    ret;

;void putchar(uint8_t attr,char s);
puts:
    push    ebp
    mov     ebp,esp
    pushad
    mov     ebx,[ebp+12]
    mov     ah,[ebp+8]
_puts_begin:
    mov     al,[ebx]
    cmp     al,0
    je       _puts_end
    push    ebx
    push    eax
    mov     ebx,eax
    and     eax,0x000000ff
    push    eax
    mov     bl,bh
    and     ebx,0x000000ff
    push    ebx
    call    putchar
    add     esp,8
    pop    eax
    pop     ebx
    inc     ebx
    jmp _puts_begin
_puts_end:
    popad
    leave
    ret

;void putchar(uint8_t attr,char s);
putchar:
    push    ebp
    mov     ebp,esp
    pushad
    mov     ah,[ebp+8]
    mov     al,[ebp+12]
;记录当前光标位置
    call    read_cursor
    movzx   ebx,bx
    cmp     al,0x0a
    je      _putchar_wrap
    cmp     al,0x0d
    je      _putchar_wrap
    shl     ebx,1
    mov     [ebx+graphicsMemoryPosition],ax
    shr     ebx,1
    inc     ebx
    jmp     _putchar_ifFull
_putchar_wrap:;换行
    xor     edx,edx
    mov     eax,ebx
    mov     ebx,80
    div     ebx
    inc     eax
    mul     ebx
    mov     ebx,eax
_putchar_ifFull:;判断是否满屏
    cmp     ebx,80*25
    jnae    _putchar_end
    mov      ebx,80*24
    mov     edi,graphicsMemoryPosition
    mov     esi,80*2+graphicsMemoryPosition
    mov     ecx,24*80
    rep     movsw
    mov      ecx,80
_putchar_clear_last_line:
    mov     word [edi],0
    add     edi,2
    loop    _putchar_clear_last_line
_putchar_end:
    call    write_cursor
    popad
    leave
    ret

;读光标位置(结果保存到bx)
read_cursor:
    push    eax
    push    ecx
    push    edx
    mov     dx,0x3d4
    mov     al,0x0e
    out     dx,al
    mov     dx,0x3d5
    in      al,dx
    mov     ah,al
    mov     dx,0x3d4
    mov     al,0x0f
    out     dx,al
    mov     dx,0x3d5
    in      al,dx
    mov     bx,ax
    pop     edx
    pop     ecx
    pop     eax
    ret
;写光标位置(位置保存在BX)
write_cursor:
    push    eax
    push    ebx
    push    ecx
    push    edx
    mov     dx,0x3d4
    mov     al,0x0e
    out     dx,al
    mov      dx,0x3d5
    mov     al,bh
    out      dx,al
    mov     dx,0x3d4
    mov     al,0x0f
    out     dx,al
    mov     dx,0x3d5
    mov      al,bl
    out     dx,al
    pop     edx
    pop      ecx
    pop      ebx
    pop      eax
    ret
