ENTRY(_start)

SECTIONS
{
    . = 0x8200;

    .text : ALIGN(4)
    {
        *(.text.entry)
        *(.text*)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    }

    .data : ALIGN(4)
    {
        *(.data*)
    }

    .bss : ALIGN(4)
    {
        *(COMMON)
        *(.bss*)
    }
}
