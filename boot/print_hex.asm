; ==========================================================
; print_hex
; Prints DX as "0x0000"
; Input: DX = value
; ==========================================================
print_hex:
    pusha

    mov ax, dx                  ; copy dx into ax
    mov di, HEX_OUT + 5         ; last digit position
    mov cx, 4                   ; start counter (4 hex digits)

.hex_loop:
    mov bl, al
    and bl, 0x0f                ; low nibble only
    cmp bl, 9                   ; Check if it is digit or letter
    jbe .digit
    add bl, 'A' - 10            ; Convert to ascii 10...15 -> 'A'....'F'
    jmp .store

.digit:
    add bl, '0'                 ; Convert to ascii 0...9 -> '0'...'9'

.store:
    mov [di], bl
    dec di
    shr ax, 4                   ; shift ax 4 positions to right
    loop .hex_loop              ; Check if cx = 0 if yes fallthrough otherwise jumb to .hex_loop

    mov bx, HEX_OUT             ; Move HEX_OUT to bx
    call print_string           ; Call print string function 

    popa
    ret


; Data
HEX_OUT: db '0x0000',0