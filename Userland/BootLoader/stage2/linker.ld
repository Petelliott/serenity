OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x0007FFFF;
    .text :
    {
        *(.start)
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        CONSTRUCTORS
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}
