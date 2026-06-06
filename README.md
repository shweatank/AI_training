# STM32F407 LED Chase — C++17, Bare Metal

Lights the four user LEDs on the **STM32F407G-DISC1** Discovery board one after
another, in a continuous chase. No vendor HAL — direct register access wrapped
in zero-cost C++17 abstractions.

## LED map (port D)

| Pin  | Colour |
|------|--------|
| PD12 | green  |
| PD13 | orange |
| PD14 | red    |
| PD15 | blue   |

## Layout

```
stm32f407_blink/
├── Makefile                     # cross-compile + flash
├── linker/STM32F407VG.ld        # 1 MB flash / 128 KB RAM
└── src/
    ├── registers.hpp            # memory-mapped RCC + GPIO
    ├── gpio.hpp                 # DigitalOut, zero-overhead pin wrapper
    ├── main.cpp                 # the chase sequence
    └── startup_stm32f407.cpp    # vector table + C-runtime init (C++)
```

## Prerequisites

Install the ARM bare-metal toolchain (this machine doesn't have it yet):

```bash
# Debian / Ubuntu
sudo apt install gcc-arm-none-eabi
# Flashing (pick one)
sudo apt install stlink-tools            # provides st-flash  -> make flash
# or use ST's STM32CubeProgrammer CLI    # provides STM32_Programmer_CLI -> make flash-cube
```

## Build & flash

```bash
make            # -> build/blink.elf, .bin, .hex  + size report
make flash      # write build/blink.bin to 0x08000000 via st-flash
make clean
```

## Notes

- Built for the Cortex-M4F with hard-float (`-mfpu=fpv4-sp-d16 -mfloat-abi=hard`).
- Exceptions and RTTI are disabled (`-fno-exceptions -fno-rtti`), as is usual
  for small embedded C++ — keeps the image tiny and avoids the unwinder.
- The delay is a calibrated busy-loop on the 16 MHz HSI default clock, so the
  blink rate is approximate. For precise timing, drive it from SysTick.
