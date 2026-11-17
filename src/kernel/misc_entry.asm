bits 32
extern kernel_main

section .text.entry         ; Dedicated section placed first by linker.ld
global _start
_start:
    call kernel_main
