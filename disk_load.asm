; ==========================================================
; load DH sectors to ES:BX from drive DL
; Input: DH = number of sectors to read
; ==========================================================

disk_load:
    push dx                     ; Store dx on stsack so later we can recall
                                ; how many sectors were request to be read

    mov ah, 0x02                ; Bios read sector function
    mov al, dh                  ; Read dh sectors
    mov ch, 0x00                ; Select cylinder 0
    mov dh, 0x00                ; Select head 0
    mov cl, 0x02                ; Start reading from second sectore (after boot sector)

    int 0x13                    ; Bios interrupt to read disk
    jc disk_error               ; Jumb if error (carry flag is set)

    pop dx                      ; Restore dx from stach so we can compare request/read
    cmp dh, al                  ; Check if same expected number is read
    jne disk_error
    ret

disk_error:

    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $

; Data
DISK_ERROR_MSG: db "Disk read error!",0
