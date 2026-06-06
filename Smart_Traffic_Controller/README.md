# Smart Traffic Light Controller (STM32F407, C++17)

Bare-metal, register-level firmware for the **STM32F407VG / STM32F407G-DISC1**
that drives a configurable traffic-light sequence and is controlled over a
USART2 serial console. The design separates **platform-independent application
logic** from **STM32 hardware drivers** behind small interfaces, so the whole
application is unit-tested on the host with no hardware.

```
R30   -> red duration   = 30 s     START  -> begin cycling
G20   -> green duration = 20 s     STOP   -> halt (IDLE)
Y5    -> yellow duration =  5 s     STATUS -> print timings + state
                                    HELP   -> list commands
```
Every valid command replies `ACK`; invalid input replies `NACK`. Commands that
change configuration or run-state also echo an updated `STATUS` line.

---

## Features

- 4 on-board LEDs: red / yellow (orange) / green signals + a blue RUN indicator.
- Configurable phase timing via USART commands.
- **Non-blocking**, timer-driven state machine — no `delay()` anywhere.
- Interrupt-driven UART RX into a lock-free ring buffer; **all parsing happens
  in the main loop**, never in the ISR.
- Wraparound-safe millisecond timing (SysTick).
- Runtime status reporting.

## Hardware

| Function        | Pin   | Notes                                   |
|-----------------|-------|-----------------------------------------|
| RED signal      | PD14  | on-board red LED                        |
| YELLOW signal   | PD13  | on-board orange LED                     |
| GREEN signal    | PD12  | on-board green LED                      |
| RUN indicator   | PD15  | on-board blue LED (lit while cycling)   |
| Console TX      | PA2   | USART2_TX, AF7                          |
| Console RX      | PA3   | USART2_RX, AF7                          |

Console line settings: **115200 8N1**. Clocks run from the 16 MHz HSI (no PLL),
so both the core (SysTick) and APB1 (USART2) are 16 MHz.

## Layout

```
firmware/
  application/         platform-independent business logic
    hal/               interfaces: ILed, IUart, IClock, ITimer
    ring_buffer.hpp    lock-free SPSC FIFO (UART RX)
    soft_timer.hpp     wraparound-safe ITimer over an IClock
    line_assembler.hpp bounded line accumulator
    format.hpp         allocation-free integer formatting
    command*.{hpp,cpp} protocol parser + processor
    traffic_light.*    RED/GREEN/YELLOW/IDLE state machine
  platform/stm32/      register-level drivers + hal adapters
    stm32f407_*.{hpp,cpp}  regs / rcc / gpio / uart
    systick_clock.*    SysTick -> IClock (+ SysTick_Handler)
    uart_console.*     interrupt RX -> IUart (+ USART2_IRQHandler)
    gpio_led.hpp       GPIO pin -> ILed
    board.hpp          pin/clock map
src/main.cpp           composition root + non-blocking super-loop
startup/               reset handler + vector table
linker/                STM32F407 linker script
tests/                 host unit tests (no hardware)
docs/                  architecture / protocol / testing
```

See [docs/architecture.md](docs/architecture.md) for the layering rationale,
[docs/protocol.md](docs/protocol.md) for the wire protocol, and
[docs/testing.md](docs/testing.md) for the test strategy.

## Build & flash

Requires the GNU Arm Embedded toolchain (`arm-none-eabi-gcc`).

```sh
make            # cross-compile -> build/traffic_controller.{elf,bin,hex,map}
make size       # section sizes
make flash      # st-flash write build/traffic_controller.bin 0x08000000
make clean
```

## Test (host)

Builds with the native `g++`; no hardware or cross toolchain needed. The
register drivers are exercised against ordinary RAM, and the application against
fake LEDs / UART / clock.

```sh
make test
```

```
========================================
Cases: 65   Checks: 231   Failures: 0
Result: PASS
========================================
```

## Usage example

```
STM32F407 Smart Traffic Light Controller (C++17)
Type HELP for commands.
STATUS STATE=IDLE RUN=0 R=30 G=20 Y=5 REMAIN=0
> R15
ACK
STATUS STATE=IDLE RUN=0 R=15 G=20 Y=5 REMAIN=0
> START
ACK
STATUS STATE=RED RUN=1 R=15 G=20 Y=5 REMAIN=15
> STOP
ACK
STATUS STATE=IDLE RUN=0 R=15 G=20 Y=5 REMAIN=0
> FOO
NACK
```

## Design constraints

C++17, freestanding. No exceptions, no RTTI, no dynamic allocation
(`new`/`delete`/`malloc`/`free`), no `std::string`/`iostream` in firmware, no
recursion. Fixed-size statically allocated buffers throughout; all indices and
lengths bounds-checked. Interrupt handlers are minimal (read byte, enqueue) and
never parse or transmit. Strongly-typed `enum class`, `constexpr` over macros,
and `const`-correct interfaces
