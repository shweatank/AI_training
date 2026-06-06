# Testing

All tests run on the **host** with the native `g++` — no hardware, no cross
toolchain, no emulator. The same source that compiles for the STM32 is linked
against fakes and ordinary RAM.

```sh
make test
```

A passing run ends with:

```
========================================
Cases: 65   Checks: 231   Failures: 0
Result: PASS
========================================
```

`make test` exits non-zero on any failure, so it is CI-friendly.

## How hardware is avoided

- **Register drivers** (`Gpio`, `Uart`) take a *pointer to a register-block
  struct*. Tests allocate that struct in normal RAM and assert on the resulting
  field values — exactly the bits the silicon would latch.
- **Application code** depends only on the `hal` interfaces. Tests inject fakes
  from `tests/mocks.hpp`:
  - `FakeClock` — a manually advanced millisecond counter,
  - `FakeLed` — records its on/off state,
  - `FakeUart` — captures TX as a string, serves queued RX bytes.

## Coverage map

| Area                       | File                         | Notable cases |
|----------------------------|------------------------------|---------------|
| Ring buffer                | `test_ring_buffer.cpp`       | FIFO order, full→drop, index **wraparound**, clear |
| Software timer             | `test_soft_timer.cpp`        | exact-expiry boundary, stop, **counter wraparound** |
| Line assembler             | `test_line_assembler.cpp`    | CR/LF terminate, blank lines, **overflow** + recovery, exact-capacity |
| Command parser             | `test_command_parser.cpp`    | valid R/G/Y/keywords, case-insensitive, trim, invalid tokens, **duration bounds & overflow** |
| State machine              | `test_traffic_light.cpp`     | IDLE start, **RED→GREEN→YELLOW→RED** transitions, STOP, active-phase re-arm, REMAIN rounding |
| Command processor (E2E)    | `test_command_processor.cpp` | **ACK/NACK**, STATUS echo, START/STOP, HELP, lowercase, out-of-range NACK leaves state intact |
| GPIO driver                | `test_gpio.cpp`              | MODER/OTYPER/PUPDR/OSPEEDR/AFR fields, BSRR set/reset/toggle, **out-of-range pin ignored** |
| USART driver               | `test_uart.cpp`              | BRR formula, 8N1 / 9-bit-even / odd-parity config, RXNEIE toggle, TX/RX paths |

The mandated test scenarios are all present: valid commands, invalid commands,
boundary timing, **tick wraparound**, **buffer-overflow handling**, and **state
transitions**.

## Notes on claims

The PASS summary above is reproduced from an actual `make test` run on this
tree. Do not edit the numbers by hand; re-run `make test` and paste the real
output if the suite changes.

## What is not covered on host

- The reset/startup path, vector table, and the `SysTick_Handler` /
  `USART2_IRQHandler` ISR bodies run only on target (or under an instruction
  emulator). Their logic is intentionally a one-liner each, delegating to the
  host-tested `SysTickClock` counter and `UartConsole::onRxInterrupt()`.
- Real baud-rate timing and electrical behaviour require the board; bring up
  with a 3V3 USB-UART adapter on PA2/PA3 at 115200 8N1.
