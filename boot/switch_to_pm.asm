[bits 16]
; Switch to protected mode
switch_to_pm:
    cli                                     ; Disable interrupts otherwise they will wreak havoc before we set up ISR

    lgdt [gdt_descriptor]                   ; Load global decriptor table, which defines our segments

    mov eax, cr0                            ; To move to protected mode we set first byte of CR0 register to 1
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:init_pm                    ; We make a far jump to our 32-bit code. This also forces cpu to flush its chache of pre-fetched
                                            ; and real-mode decoded instructions, which can cause problems if not flushed

[bits 32]
;Initialize registers and stack once in pm
init_pm:
    mov ax, DATA_SEG                        ; Once we enter protected mode we need to initialize our register because
    mov ds, ax                              ; our old segment is meaningless, so we point them to our segments defined
    mov ss, ax                              ; in our GDT
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000                        ; Update our stack to top of the free space
    mov esp, ebp

    call BEGIN_PM                           ; Call to begin of protected mode