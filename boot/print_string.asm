; ==========================================================
; print_string
; Prints a null-terminated string using BIOS teletype
; Input: BX = address of string
; ==========================================================
print_string:
    pusha           ; Push all to stack

.print_loop:
    mov al, [bx]    ; Load 
    cmp al, 0
    jz .end
    mov ah, 0x0e
    int 0x10
    inc bx
    jmp .print_loop

.end:
    popa
    ret

; ==========================================================
; print_newline
; Prints CRLF (newline)
; ==========================================================
print_newline:
    pusha
    mov ah, 0x0e

    mov al, 0x0d        ; \r
    int 0x10
    mov al, 0x0a        ; \n
    int 0x10

    popa
    ret

