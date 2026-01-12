; ==========================================================
; GDT - Global Descriptor Table
; ==========================================================

gdt_start:

; Adding mandatory null decriptor using dd "define double word"
gdt_null:
    dd 0x0
    dd 0x0

; Code segment descriptor
gdt_code:
    ; base = 0x0, limit = 0xffff
    ; 1st flags: present = 1, privilige = 00, descriptor type = 1 -> 1001b
    ; type flags: code = 1, conforming = 0, readable = 1, accessed = 0 -> 1010b
    ; 2nd flags: granularity = 1, operation size = 1, 64-bit code segment = 0, AVL = 0 -> 1100b
    dw 0xffff       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; base (bits 16-23)
    db 10011010b    ; 1st flag and type flags 
    db 11001111b    ; 2nd flags and limit (bits 16-19)
    db 0x0          ; Base (bits 24-32)

gdt_data:
    ; base = 0x0, limit = 0xffff
    ; 1st flags: present = 1, privilige = 00, descriptor type = 1 -> 1001b
    ; type flags: code = 0, conforming = 0, readable = 1, accessed = 0 -> 0010b
    ; 2nd flags: granularity = 1, operation size = 1, 64-bit code segment = 0, AVL = 0 -> 1100b
    dw 0xffff       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; base (bits 16-23)
    db 10010010b    ; 1st flag and type flags 
    db 11001111b    ; 2nd flags and limit (bits 16-19)
    db 0x0          ; Base (bits 24-32)

gdt_end:        ; Reason for label at the end of GDT is so we can have assembler calculater the size
                ; of the GDT for GDT descriptor below

; GTD descriptor (GDT size (16 bits) + GDT address (32 bits))
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Start address of our gdt

; Define some handy constants for the GDT segment descriptor offsets, which
; are what segment registers must contain when in protected mode. For example ,
; when we set DS = 0x10 in PM , the CPU knows that we mean it to use the
; segment described at offset 0x10 ( i.e. 16 bytes ) in our GDT, which in our
; case is the DATA segment (0x0->NULL ; 0x08->CODE ; 0x10->DATA)
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start