/* STM32F407 Linker Script
   For STM32F407VG with ARM Cortex-M4
*/

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K    /* 1MB Flash */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 192K     /* 192KB SRAM */
    CCM (rwx)   : ORIGIN = 0x10000000, LENGTH = 64K      /* 64KB CCM */
}

/* Define output sections */
SECTIONS
{
    /* Program code and read-only data */
    .text :
    {
        KEEP(*(.vectors))           /* Vector table */
        *(.text)                    /* .text sections */
        *(.text*)                   /* .text* sections */
        *(.rodata)                  /* Read-only data */
        *(.rodata*)                 /* Read-only data* */
        
        /* C++ static objects */
        *(.init_array)
        *(.fini_array)
        
        _etext = .;                 /* End of text section */
    } > FLASH

    /* Initialized data (copied to RAM at startup) */
    .data :
    {
        _sdata = .;                 /* Start of data section */
        *(.data)
        *(.data*)
        _edata = .;                 /* End of data section */
    } > RAM AT > FLASH

    /* Uninitialized data (zeroed at startup) */
    .bss :
    {
        _sbss = .;                  /* Start of BSS section */
        *(.bss)
        *(.bss*)
        *(COMMON)
        _ebss = .;                  /* End of BSS section */
    } > RAM

    /* Stack and heap */
    .stack (COPY):
    {
        . = ALIGN(8);
        . += 0x2000;                /* 8KB stack */
        _estack = .;
    } > RAM

    /* CCM RAM section (if needed) */
    .ccm (NOLOAD) :
    {
        *(.ccm)
    } > CCM

    /* Remove unused sections */
    /DISCARD/ :
    {
        libc.a (*)
        libm.a (*)
        libgcc.a (*)
    }
}

/* Define symbols for use in C code */
__text_start = ORIGIN(FLASH);
__text_end = ORIGIN(FLASH) + LENGTH(FLASH);
__ram_start = ORIGIN(RAM);
__ram_end = ORIGIN(RAM) + LENGTH(RAM);
