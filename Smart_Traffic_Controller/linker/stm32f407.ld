/*
 * stm32f407.ld - Linker script for STM32F407VG
 *   1 MB Flash @ 0x08000000
 *   128 KB main SRAM @ 0x20000000 (+ 64 KB CCM @ 0x10000000, unused here)
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Top of the main SRAM: initial stack pointer. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Minimum heap/stack headroom check. */
_Min_Heap_Size  = 0x200;
_Min_Stack_Size = 0x400;

SECTIONS
{
    /* Vector table + code in flash. */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ARM exception unwind tables (kept minimal; -fno-exceptions still emits stubs). */
    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    /* C++ static constructor / init array. */
    .init_array :
    {
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        __init_array_end = .;
        . = ALIGN(4);
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        . = ALIGN(4);
    } > FLASH

    /* .data load address in flash. */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* Heap base used by newlib's _sbrk (referenced via nosys.specs even though
       this firmware performs no dynamic allocation). */
    PROVIDE(end = _ebss);
    PROVIDE(_end = _ebss);

    /* Reserve heap + stack space and fail the link if it doesn't fit. */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM

    /DISCARD/ : { *(.comment) }
}
