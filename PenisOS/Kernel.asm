section .text
    align 4
    dd 0x1BADB002
    dd 0
    dd - (0x1BADB002+0)
    
    global      _start
    global  gdt_flush
    extern      Entry
_start:
    call    Entry
exit:
    jmp     _start
gdt_flush:
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax 
    mov     fs, ax
    mov     gs, ax 
    mov     ss, ax 
    jmp     0x08:_flush2     
_flush2:
    ret
