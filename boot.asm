[org 0x7c00]

    mov bp, 0x9000                              ; Set stack to top of free memory
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print_string

    call switch_to_pm

    jmp $

%include "print_string.asm"
%include "print_string_pm.asm"
%include "gdt.asm"
%include "switch_to_pm.asm"

[bits 32]
; This is our start position after switiching and initializing protected mode
BEGIN_PM:

    mov ebx, MSG_PROTECTED_MODE
    call print_string_pm

    jmp $

; Global variables
MSG_REAL_MODE db "Started in 16-bit Real mode", 0
MSG_PROTECTED_MODE db "Successfully started 32-bit protected mode", 0

; Bootsector padding
times 510-($-$$) db 0
dw 0xAA55

