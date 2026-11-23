bits 16
org 0x7c00

jmp _start

boot_disk db 0

print_char:
    mov ah, 0x0e
    int 10h
    ret
print_string:
.print_loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .print_loop
.done:
    ret

disk_fail:
    mov si, read_error_msg
    call print_string
    jmp $
reading_msg    db "Loading sectors..", 0
read_error_msg db "Disk Read Error", 0

_start:
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov sp, 0x7c00

    mov ax, 0x0000
    mov es, ax          ; Destination Segment ES=0x0000
    mov bx, 0x7e00      ; Destination Offset BX=0x7E00
    mov ch, 0x00        ; Cylinder 0
    mov cl, 0x02        ; Sector 2
    mov dh, 0x00        ; Head 0
    mov ah, 0x02
    mov al, 15
    int 0x13
    jc disk_fail

    jmp 0x7e00

times 510-($-$$) db 0
dw 0xAA55