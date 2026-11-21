bits 32
extern _bss_start
extern _bss_end
extern kernel_main

section .text.entry
global _start
_start:
    mov esp, stack_top

    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb

    call kernel_main
.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top: