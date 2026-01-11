; Load kernel from disk to KERNEL_OFFSET
KERNEL_OFFSET equ 0x1000                        ; This is memory offset where we will load our kernel

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET                   ; Set up parameters for our disk loading function, we will load first 15
    mov dh, 15                              ; sectors from boot disk to address KERNEL_OFFSET
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret

; Data
MSG_LOAD_KERNEL         db "Loading kernel into memory", 0
