[org 0x7c00]
[bits 16]
KERNEL_OFFSET equ 0x1000                        ; This is memory offset where we will load our kernel

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov bp, 0x9000                              ; Set stack to top of free memory
    mov sp, bp
    cld

    mov bx, MSG_REAL_MODE
    call print_string

    call load_kernel

    call switch_to_pm

    jmp $

%include "print_string.asm"
%include "print_string_pm.asm"
%include "gdt.asm"
%include "switch_to_pm.asm"
%include "disk_load.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET                   ; Set up parameters for our disk loading function, we will load first 15
    mov dh, 15                              ; sectors from boot disk to address KERNEL_OFFSET
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret


[bits 32]
; This is our start position after switiching and initializing protected mode
BEGIN_PM:

    mov ebx, MSG_PROTECTED_MODE
    call print_string_pm

    call KERNEL_OFFSET

    jmp $

; Global variables
BOOT_DRIVE              db 0
MSG_REAL_MODE           db "Started in 16-bit Real mode", 0
MSG_PROTECTED_MODE      db "Successfully started 32-bit protected mode", 0
MSG_LOAD_KERNEL         db "Loading kernel into memory", 0
; Bootsector padding
times 510-($-$$) db 0
dw 0xAA55

