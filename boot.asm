[org 0x7c00]

xor ax, ax
mov ds, ax
mov es, ax


mov [BOOT_DRIVE], dl

mov bp, 0x8000
mov sp, bp


mov bx, 0x9000
mov dh, 5
mov dl, [BOOT_DRIVE]
call disk_load

mov dx, [0x9000]
call print_hex

mov dx, [0x9000 + 512]
call print_hex

jmp $

%include "print_string.asm"
%include "print_hex.asm"
%include "disk_load.asm"

BOOT_DRIVE: db 0

times 510-($-$$) db 0
dw 0xAA55

times 256 dw 0xdada   ; sector 2
times 256 dw 0xface   ; sector 3
times 256 dw 0xbeef   ; sector 4
times 256 dw 0xcafe   ; sector 5
times 256 dw 0x1337   ; sector 6