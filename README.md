# GitHub Copilot Instructions: STM32 Smart Traffic Light Controller (C++17)

## Project Context

This repository contains modern embedded C++17 firmware for STM32 Cortex-M4 microcontrollers.

Reference target:

* STM32F407VG
* STM32F407 Discovery Board
* ARM Cortex-M4
* Bare-metal register-level drivers
* GNU Arm Embedded Toolchain
* Makefile build system
* USART2 debug console
* Traffic Light LEDs using GPIO
* Timer-driven non-blocking scheduler

---

# Project: Smart Traffic Light Controller

## Features

* 4 LEDs represent traffic signals
* Configurable timing using USART commands
* Non-blocking LED scheduling
* Real-time state machine
* USART command parser
* Runtime status reporting

Supported commands:

```text
R30   -> Red light duration = 30 sec
G20   -> Green light duration = 20 sec
Y5    -> Yellow light duration = 5 sec
START -> Start controller
STOP  -> Stop controller
STATUS -> Print current timings and active state
HELP  -> Print supported commands
```

All valid commands must return:

```text
ACK
```

Invalid commands must return:

```text
NACK
```

State-changing commands must also print updated STATUS.

---

# Mandatory Design Rules

When generating or modifying code:

1. Use C++17
2. Do not use exceptions
3. Do not use RTTI
4. Do not use dynamic memory allocation
5. Do not use:

   * new
   * delete
   * malloc
   * free
6. Do not use std::string or iostream
7. Use fixed-size statically allocated buffers
8. Validate all indices and buffer lengths
9. Do not use recursion
10. Do not block inside interrupts
11. Do not use printf inside interrupts
12. Do not perform UART parsing in ISR context
13. Keep interrupt handlers minimal
14. Use wraparound-safe timing logic
15. Keep HAL/register access isolated to platform drivers
16. Keep application logic platform-independent
17. Always validate peripheral configuration
18. Use strongly typed enum class
19. Prefer constexpr over macros
20. Use const correctness

---

# Architecture

Application logic must be separated from hardware drivers.

Use interfaces like:

```cpp
class ILed;
class IUart;
class ITimer;
class IClock;
```

Application layer must not directly access:

* GPIO registers
* USART registers
* HAL APIs

Hardware-specific code belongs in:

```text
firmware/platform/stm32
```

Business logic belongs in:

```text
firmware/application
```

---

# Traffic Light State Machine

States:

```text
RED
YELLOW
GREEN
IDLE
```

Transitions must be timer-driven.

No blocking delays allowed.

Correct timing pattern:

```cpp
if (static_cast<std::uint32_t>(now - previous_tick) >= interval)
{
    // state transition
}
```

Do not use delay-based scheduling.

---

# USART Rules

USART interrupt handlers may:

* Read received byte
* Push byte into ring buffer
* Update minimal flags/counters

USART interrupt handlers must NOT:

* Parse commands
* Format strings
* Execute state-machine logic
* Perform blocking transmission

Command parsing must occur in main loop or task scheduler.

---

# Driver Rules

GPIO driver:

* Configure MODER
* Configure OTYPER
* Configure OSPEEDR
* Configure PUPDR
* Configure AFRL/AFRH

USART driver:

* Support polling TX/RX
* Support interrupt RX
* Support configurable baudrate
* Support ring buffer

Timer driver:

* Provide millisecond tick
* Non-blocking scheduling support

---

# Build Rules

Firmware must compile using:

```text
-std=c++17
-fno-exceptions
-fno-rtti
-ffunction-sections
-fdata-sections
```

Build outputs:

```text
ELF
BIN
HEX
MAP
```

Enable strong warnings for project code.

---

# Code Style

Prefer:

* enum class
* constexpr
* nullptr
* explicit constructors
* small focused functions
* dependency injection
* fixed-size buffers
* static allocation
* register-level clarity

Avoid:

* hidden globals
* giant classes
* unchecked sprintf
* blocking delays
* unnecessary templates
* hardware logic mixed with business logic

---

# Documentation Rules

Update documentation when behavior changes:

```text
README.md
docs/architecture.md
docs/protocol.md
docs/testing.md
```

Public APIs should include Doxygen comments.

---

# Testing Rules

Add/update tests for:

* Valid commands
* Invalid commands
* Boundary timing cases
* Tick wraparound
* Buffer overflow handling
* State transitions

Do not claim tests passed unless actual command output confirms success.

---

# Interview Focus

This project demonstrates:

* Embedded C++17 design
* Real-time state machines
* Register-level STM32 programming
* Non-blocking scheduling
* USART protocol handling
* Interrupt-safe firmware architecture
* Production-style embedded software structure
