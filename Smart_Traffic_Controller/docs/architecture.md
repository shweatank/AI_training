# Architecture

## Goals

1. **Hardware-independent business logic.** The traffic-light state machine and
   protocol handling contain no register access and compile/run identically on
   the target and on a host PC.
2. **Testability.** Every layer is exercised by host unit tests with zero
   hardware: drivers against ordinary RAM, application against fakes.
3. **Interrupt-safe, non-blocking.** No busy-wait delays in the control flow;
   ISRs do the bare minimum.

## Layers

```
            +-------------------------------------------------+
            |                  src/main.cpp                   |
            |        composition root + super-loop            |
            +------------------+--------------------+---------+
                               | depends on (interfaces only)
                  +------------v------------+   +--v--------------------+
                  |     application/        |   |   application/hal/    |
                  |  TrafficLight           |   |  ILed   IUart         |
                  |  CommandParser/Processor |-->|  IClock ITimer        |
                  |  SoftTimer LineAssembler |   |  (pure abstractions)  |
                  |  RingBuffer  format      |   +--^--------------------+
                  +-------------------------+      | implemented by
                                                   |
                  +--------------------------------+----------------+
                  |            platform/stm32/                       |
                  |  GpioLed(ILed)  UartConsole(IUart)               |
                  |  SysTickClock(IClock)  SoftTimer uses IClock     |
                  |  Gpio / Uart / Rcc register drivers              |
                  |  SysTick_Handler / USART2_IRQHandler             |
                  +--------------------------------------------------+
```

- **`application/`** depends only on `application/hal/` interfaces. It never
  includes a platform header. Namespace `app`.
- **`application/hal/`** holds the four abstractions: `ILed`, `IUart`,
  `IClock`, `ITimer`. Namespace `hal`.
- **`platform/stm32/`** holds register definitions and drivers (namespace
  `stm32f407`) plus thin adapters that implement the `hal` interfaces:
  - `GpioLed` → `ILed` (one GPIO pin),
  - `UartConsole` → `IUart` (polled TX, interrupt-buffered RX),
  - `SysTickClock` → `IClock` (1 kHz SysTick counter).
- **`src/main.cpp`** is the only translation unit that includes both worlds. It
  constructs the concrete drivers, injects them into the application through the
  interfaces, and runs the loop.

This is classic dependency injection: dependencies point *inward* toward the
abstractions, so the application has no compile-time knowledge of the STM32.

## Timing model

`SysTickClock` increments a free-running 32-bit millisecond counter in the
SysTick interrupt. `SoftTimer` (an `ITimer`) is armed with a duration and polled
with `expired()`, using unsigned-difference arithmetic:

```cpp
elapsed = (uint32_t)(now - start);   // correct across the 2^32 ms wrap
expired = elapsed >= duration;
```

The `TrafficLight` state machine asks only `timer.expired()` — it has no notion
of "now", delays, or the clock — which keeps it trivial to test and free of
wraparound bugs.

## Data flow (RX)

```
USART2 RXNE IRQ ──► USART2_IRQHandler ──► UartConsole::onRxInterrupt()
                                              │ (read DR, push)
                                              ▼
                                        RingBuffer<uint8_t>      [ISR boundary]
                                              │ pop (main loop)
                                              ▼
   main loop ─► LineAssembler ─(complete line)─► CommandProcessor
                                                    │ parse + apply
                                                    ├─► TrafficLight (state)
                                                    └─► IUart (ACK/NACK/STATUS)
```

The ISR only reads the data register and enqueues one byte. Parsing, formatting,
and transmission all happen in the main loop, satisfying the interrupt rules.

## Concurrency

The single shared structure across the ISR/main boundary is the RX
`RingBuffer`. It is single-producer (ISR `push`) / single-consumer (main `pop`)
with `volatile` head/tail indices; on a single Cortex-M core with word-atomic
index writes this needs no critical section. A full buffer drops the newest
byte rather than blocking the ISR.

## Memory

No heap. All buffers are members or stack locals with compile-time sizes
(`RingBuffer<…,N>`, `LineAssembler<…>`, status-format scratch). The linker
script reserves a minimum stack and fails the link if RAM is exhausted.
