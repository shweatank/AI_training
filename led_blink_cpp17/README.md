# STM32F407 LED Blink — Embedded C++17

## Original Request (Prompt)

> create a separe folder and generate embedded c++17 standard using stm32f407
> goio_driver led blinking with unitest cases and makefile also ...attach to my
> diretory and compile run and flash into the board stm32f407 i am connected the
> board to my pc.

### Follow-up requests
> - dump my code if any bugs, display those, then correct the bugs and again
>   flash my corrected version code and execute on board.
> - check unit test cases how many failed and how many passed and display all the
>   bugs where test cases failed.
> - add readme file which consists of my given prompt at first.

---

A self-contained GPIO driver + LED-blinking demo for the STM32F407
(STM32F4-Discovery), written in **embedded C++17**, with **host unit tests**
(GoogleTest) and a **Makefile** for cross-compiling and flashing via ST-Link.

## Layout

```
led_blink_cpp17/
├── include/
│   ├── gpio.hpp                 # GPIO driver API (testable, register block injected)
│   └── stm32f407_registers.hpp  # GPIO + RCC register definitions
├── src/
│   ├── gpio.cpp                 # GPIO driver implementation
│   ├── main.cpp                 # LED blink app (PD12-PD15 chase + blink)
│   └── startup.cpp              # vector table, Reset_Handler, .data/.bss + ctors
├── tests/
│   ├── gpio_test.cpp            # GoogleTest cases against a fake register block
│   └── Makefile                 # host test build
├── stm32f407.ld                 # linker script (1 MB FLASH / 128 KB RAM)
├── Makefile                     # firmware build / flash / test
└── README.md
```

## LED mapping (STM32F4-Discovery)

| LED | Color  | Pin  |
|-----|--------|------|
| LD4 | green  | PD12 |
| LD3 | orange | PD13 |
| LD5 | red    | PD14 |
| LD6 | blue   | PD15 |

## Build & flash

```bash
make            # cross-compile -> build/led_blink.elf/.hex/.bin + size
make flash      # flash to the board via OpenOCD/ST-Link, verify, reset, run
make size       # show firmware size
make erase      # mass-erase the chip
make clean      # remove build artifacts
```

## Unit tests (host)

The driver takes a `GpioRegisters*` at construction. Tests point it at a plain
struct in host RAM and assert on the exact register bits written — no hardware
needed.

```bash
make test       # builds & runs GoogleTest suite on the host
```

## Why it is testable

`Gpio::at(Port)` enables the RCC clock and binds to the real peripheral on the
MCU. `Gpio(GpioRegisters*)` lets tests inject a fake block. Same `gpio.cpp`
compiles for both the target and the host.

## Requirements

- `arm-none-eabi-g++` (tested with 13.2)
- `openocd` (with `interface/stlink.cfg` + `target/stm32f4x.cfg`)
- `g++` + GoogleTest (`libgtest`) for host tests
