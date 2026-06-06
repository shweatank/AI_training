/* 
 * Linker script for STM32F407 
 * Memory layout: Flash: 1MB (0x08000000), SRAM: 192KB (0x20000000)
 */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}

ENTRY(Reset_Handler)

SECTIONS
{
    /* Code section */
    .text :
    {
        KEEP(*(.vectors))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.gnu.linkonce.t.*)
        
        /* C++ support */
        KEEP(*(.init))
        KEEP(*(.fini))
        KEEP(*(.eh_frame))
        
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Initialized data section */
    .data :
    {
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    /* Uninitialized data section */
    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* C++ support sections */
    .ctors :
    {
        KEEP(*crtbegin.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*crtend.o(.ctors))
    } > FLASH

    .dtors :
    {
        KEEP(*crtbegin.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*crtend.o(.dtors))
    } > FLASH

    /* Stack location at end of SRAM */
    . = ALIGN(4);
    _heap_start = .;
    _stack_end = ORIGIN(SRAM) + LENGTH(SRAM) - 4;
    PROVIDE(_stack = _stack_end);
}
