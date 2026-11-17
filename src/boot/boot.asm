bits 16
org 0x7e00

jmp _start

entry_msg db "Unlocker passed, entering stage-2..", 0

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

print_string_32:
    .print_loop:
        lodsb
        test al, al
        jz .done
        mov ah, 0x0A
        mov [edi], ax
        add edi, 2
        jmp .print_loop
    .done:
        ret

_start:
    mov si, entry_msg
    call print_string
    mov ah, 0x02
    mov dh, 0
    mov dl, 0
    mov bh, 0
    int 10h

    xor ax, ax
    mov es, ax
    mov ds, ax

    ; Enable A20 Line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load Global Descriptor Table
    cli
    lgdt[gdt_descriptor]

    ; Switch to 32-bit mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:init_pm

    ; ------------- GDT DECLARATION -------------
    gdt_start:
        dq 0x0 ; Null descriptor
    
    gdt_code:
        dw 0xFFFF    ; Limit (bits 0-15)
        dw 0x0       ; Base  (bits 0-15)
        db 0x0       ; Base  (bits 16-23)
        db 10011010b ; Access byte: Present, Ring 0, Code, Executable, Readable
        db 11001111b ; Flags (4KB granularity, 32-bit) + Limit (bits 16-19)
        db 0x0       ; Base  (bits 24-31)

    gdt_data:
        dw 0xFFFF    ; Limit (bits 0-15)
        dw 0x0       ; Base  (bits 0-15)
        db 0x0       ; Base  (bits 16-23)
        db 10010010b ; Access byte: Present, Ring 0, Data, Writable
        db 11001111b ; Flags (4KB granurality, 32-bit) + Limit (bits 16-19)
        db 0x0       ; Base (bits 24-31)
    gdt_end:

    gdt_descriptor:
        dw gdt_end - gdt_start - 1 ; Size of GDT minus 1
        dd gdt_start               ; 32-bit linear address of GDT
    
    CODE_SEG equ gdt_code - gdt_start
    DATA_SEG equ gdt_data - gdt_start

    ; ------------- 32-BIT PROTECTED MODE ENTRY -------------
    bits 32
    init_pm:
        ; Update all segment registers to point to the 32-bit data selector
        mov ax, DATA_SEG
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        ; Set up a new 32-bit stack pointer
        mov ebp, 0x90000
        mov esp, ebp



        mov edi, 0xB8000
        mov ecx, 80 * 25
        mov ax, 0x0F20
        rep stosw

        jmp 0x8200 ; Jump to kernel
        

times 1024-($-$$) db 0