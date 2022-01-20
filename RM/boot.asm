section .text
    mov     [disk], dl

    ; all regs = 0
    xor     dx, dx
    xor     cx, cx
    xor     bx, bx
    xor     ax, ax

    ; set up stack
    mov     bp, 1000
    mov     sp, bp          ; place the stack pointer to the base pointer
    pusha 
    popa                    ; test out the stack

    
; makes this code bootable on the drive
boot:
    times 510-($-$$) db 0
    dw  0xAA55
disk: db 0
    
