[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f
; ==========================================================
; prints a null - terminated string
; Input: EBX = address of string
; ==========================================================
; TO-DO: Add cell position callculation
; Calculating position "0xb8000 + 2 * (row * 80 + col)"
print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY                   ; Set edx to start of video memory

print_string_pm_loop:
    mov al, [ebx]                           ; Store char at EBX to AL
    mov ah, WHITE_ON_BLACK                  ; Store attribute to AH

    cmp al, 0                               ; Check if end of string
    je print_string_pm_done

    mov [edx], ax                           ; Store char and attribute to current video memory location

    add ebx, 1                              ; Increment EBX to point to next char
    add edx, 2                              ; Increment edx to point to next cell in video memory

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret