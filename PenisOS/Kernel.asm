section .text
    align 4
    dd 0x1BADB002
    dd 0
    dd - (0x1BADB002+0)
    
    extern  Entry
    global  _start
_start:
    call    Entry
    hlt