; Macro for ISRs that DON'T push an error code
%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0                      ; Push dummy error code
    push %1                     ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for ISRs that DO push an error code
%macro isr_err_stub 1
isr_stub_%+%1:
    ; CPU already pushed error code
    push %1                     ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Common ISR stub - saves context, calls handler, restores context
isr_common_stub:
    ; Save all general-purpose registers
    pushad                      ; Pushes EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment
    mov ax, 0x10                ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the C exception handler
    ; Stack contains: interrupt_num, error_code
    call exception_handler

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general-purpose registers
    popad

    ; Clean up error code and interrupt number
    add esp, 8

    ; Return from interrupt
    ire

extern exception_handler

; CPU Exception Handlers (0-31)
isr_no_err_stub 0   ; Division By Zero
isr_no_err_stub 1   ; Debug
isr_no_err_stub 2   ; Non Maskable Interrupt
isr_no_err_stub 3   ; Breakpoint
isr_no_err_stub 4   ; Overflow
isr_no_err_stub 5   ; Bound Range Exceeded
isr_no_err_stub 6   ; Invalid Opcode
isr_no_err_stub 7   ; Device Not Available
isr_err_stub    8   ; Double Fault
isr_no_err_stub 9   ; Coprocessor Segment Overrun
isr_err_stub    10  ; Invalid TSS
isr_err_stub    11  ; Segment Not Present
isr_err_stub    12  ; Stack-Segment Fault
isr_err_stub    13  ; General Protection Fault
isr_err_stub    14  ; Page Fault
isr_no_err_stub 15  ; Reserved
isr_no_err_stub 16  ; x87 Floating-Point Exception
isr_err_stub    17  ; Alignment Check
isr_no_err_stub 18  ; Machine Check
isr_no_err_stub 19  ; SIMD Floating-Point Exception
isr_no_err_stub 20  ; Virtualization Exception
isr_no_err_stub 21  ; Reserved
isr_no_err_stub 22  ; Reserved
isr_no_err_stub 23  ; Reserved
isr_no_err_stub 24  ; Reserved
isr_no_err_stub 25  ; Reserved
isr_no_err_stub 26  ; Reserved
isr_no_err_stub 27  ; Reserved
isr_no_err_stub 28  ; Reserved
isr_no_err_stub 29  ; Reserved
isr_err_stub    30  ; Security Exception
isr_no_err_stub 31  ; Reserved

; Export the ISR stub table
global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep