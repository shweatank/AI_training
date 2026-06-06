```text
Act as a principal embedded-software architect with expertise in:

* Embedded C++11
* ARM Cortex-M
* STM32F4
* STM32Cube HAL
* Interrupt-safe firmware
* Timer peripherals
* UART protocols
* State-machine design
* CMake and GNU Arm Embedded Toolchain
* Host-based unit testing
* Static analysis and technical documentation

Create a complete, production-style STM32 firmware project in a new directory named:

stm32_digital_stopwatch

Do not create or modify files outside this directory.

====================================================================
1. TARGET PLATFORM
====================================================================

Use the following default target:

Board: STM32F407G-DISC1
MCU: STM32F407VG
Core: ARM Cortex-M4F

Running LED:
PD12 (Green LED)

Stopped LED:
PD14 (Red LED)

UART: USART2
UART TX: PA2
UART RX: PA3

Baud Rate: 115200
UART Framing: 8 Data Bits, No Parity, 1 Stop Bit

Toolchain:
arm-none-eabi-gcc
arm-none-eabi-g++

Language Standard:
C++11

Hardware Framework:
STM32CubeF4 HAL

Debugger/Programmer:
ST-LINK with OpenOCD

Keep all board-specific definitions inside a platform abstraction layer so the application can later be ported to other STM32 boards.

====================================================================
2. APPLICATION BEHAVIOR
====================================================================

Implement a Digital Stopwatch operated through UART text commands.

Commands are ASCII lines terminated by '\n' or '\r\n'.

Required Commands:

START
STOP
RESET
RESUME
STATUS
HELP

Command matching may be case-insensitive.

--------------------------------------------------------------------
STOPWATCH FUNCTIONALITY
--------------------------------------------------------------------

START
- Starts stopwatch from 00:00:00.000
- If already running, return an error

STOP
- Pauses stopwatch
- Current elapsed time must be preserved

RESET
- Clears elapsed time
- Returns stopwatch to idle state

RESUME
- Continues counting from the previously stopped value

STATUS
- Reports stopwatch state and elapsed time

HELP
- Displays supported commands

--------------------------------------------------------------------
COMMAND RESPONSES
--------------------------------------------------------------------

Use clear machine-readable responses.

Examples:

START
ACK command=START state=RUNNING

STATUS
STATUS state=RUNNING time=00:00:05.125

STOP
ACK command=STOP state=STOPPED
STATUS state=STOPPED time=00:00:05.125

RESUME
ACK command=RESUME state=RUNNING

RESET
ACK command=RESET state=IDLE
STATUS state=IDLE time=00:00:00.000

HELP
ACK command=HELP

UNKNOWN
NACK code=UNKNOWN_COMMAND command=UNKNOWN

Send an immediate ACK or NACK for every command.

Send a STATUS response after every successful state-changing command.

====================================================================
3. TIMING DESIGN
====================================================================

The stopwatch must be fully non-blocking.

Do not use:

HAL_Delay()

inside the application state machine.

Use a hardware timer interrupt that generates a 1 ms system tick.

Required Timing Behavior:

* Stopwatch resolution = 1 ms
* Elapsed time maintained in milliseconds
* START begins timing immediately
* STOP freezes current value
* RESUME continues from frozen value
* RESET clears all counters
* Timer operation must remain accurate during UART activity
* Timing implementation must work correctly when the 32-bit tick counter wraps around

--------------------------------------------------------------------
TIME FORMAT
--------------------------------------------------------------------

Format all displayed time values as:

HH:MM:SS.mmm

Examples:

00:00:00.001
00:00:05.125
00:01:25.347
01:15:42.999

====================================================================
4. TIMER ARCHITECTURE
====================================================================

Implement stopwatch timing using a dedicated hardware timer.

Preferred Implementation:

* Timer interrupt period = 1 ms
* ISR updates millisecond counter
* ISR performs only minimal operations
* ISR must not transmit UART data
* ISR must not parse commands
* ISR must not call printf
* ISR must not execute blocking code
* Stopwatch state updates occur in main application context

The stopwatch must continue receiving UART commands while timing is active.

====================================================================
5. UART ARCHITECTURE
====================================================================

Implement interrupt-driven UART reception.

Preferred Implementation:

* UART RX interrupt receives bytes
* ISR stores bytes in a fixed-size ring buffer
* Command parsing occurs in main context
* Detect and count RX buffer overflow
* Support LF and CRLF line endings
* Ignore empty lines
* Handle command length limits safely
* Reject oversized commands

Buffered UART transmission is preferred.

If blocking transmission is used:

* Keep it completely outside interrupt context
* Clearly document limitations

====================================================================
6. GPIO STATUS INDICATORS
====================================================================

Use LEDs to indicate stopwatch state.

RUNNING STATE

PD12 (Green LED) = ON
PD14 (Red LED) = OFF

STOPPED STATE

PD12 (Green LED) = OFF
PD14 (Red LED) = ON

IDLE / RESET STATE

PD12 = OFF
PD14 = OFF

ERROR STATE

Both LEDs blink continuously using a defined error pattern.

LED control must be abstracted through an interface.

====================================================================
7. SOFTWARE ARCHITECTURE
====================================================================

Use clean layering and dependency inversion.

Suggested Modules:

Application
 ├── StopwatchController
 ├── CommandProcessor
 ├── CommandParser
 ├── StatusReporter
 └── Application

Core Interfaces
 ├── ILed
 ├── IUart
 ├── IClock
 └── ITimer

Platform Implementation
 ├── Stm32Led
 ├── Stm32Uart
 ├── Stm32Clock
 └── Stm32Timer

Reusable Utilities
 ├── RingBuffer
 ├── FixedString
 ├── TimeFormatter
 └── Result/Error Definitions

Business logic must not directly call STM32 HAL functions.

HAL calls must be restricted to the platform layer.

Application logic must be testable on a host PC without STM32 hardware.

Avoid excessive abstraction.

The code must remain understandable to an embedded engineer.

====================================================================
8. EMBEDDED C++ RULES
====================================================================

Follow these constraints:

* Use C++11
* No dynamic memory allocation during normal operation
* Do not use new
* Do not use delete
* Do not use malloc
* Do not use free
* Disable C++ exceptions
* Disable RTTI
* Avoid iostream
* Avoid std::string in firmware
* Prefer fixed-size arrays
* Use bounded buffers
* Perform bounds checking
* No recursion
* No hidden blocking loops
* Keep interrupt-shared state minimal
* Use enum class
* Use nullptr
* Use constexpr
* Use fixed-width integer types
* Check every HAL return value
* Enable strong compiler warnings
* Treat warnings as errors

====================================================================
9. PROJECT STRUCTURE
====================================================================

Create at least the following structure:

stm32_digital_stopwatch/
├── CMakeLists.txt
├── Makefile
├── CMakePresets.json
├── README.md
├── LICENSE
├── .gitignore
├── .clang-format
├── .clang-tidy
├── Doxyfile
├── CLAUDE.md
├── AGENTS.md
├── config/
│   └── project_config.hpp
├── cmake/
│   ├── arm-none-eabi-toolchain.cmake
│   └── warnings.cmake
├── firmware/
│   ├── CMakeLists.txt
│   ├── include/
│   ├── src/
│   ├── platform/
│   │   └── stm32/
│   ├── startup/
│   └── linker/
├── tests/
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── fakes/
│   └── unit/
├── docs/
│   ├── architecture.md
│   ├── command_protocol.md
│   ├── build_and_flash.md
│   ├── testing.md
│   ├── porting.md
│   └── troubleshooting.md
├── scripts/
│   ├── bootstrap.sh
│   ├── build_host_tests.sh
│   ├── build_firmware.sh
│   ├── run_tests.sh
│   ├── format.sh
│   ├── static_analysis.sh
│   ├── flash.sh
│   └── debug.sh
└── .github/
    └── workflows/
        └── ci.yml

====================================================================
10. UNIT TESTING
====================================================================

Create host-based unit tests.

Do not require STM32 hardware.

Test at least:

COMMAND PARSER

* START
* STOP
* RESET
* RESUME
* STATUS
* HELP
* Lowercase commands
* Unknown commands
* Empty command
* CRLF handling
* Overlong command

STOPWATCH CONTROLLER

* Initial state
* Start stopwatch
* Stop stopwatch
* Resume stopwatch
* Reset stopwatch
* Correct millisecond accumulation
* Tick wraparound
* Multiple start attempts
* Multiple stop attempts

TIME FORMATTER

* 1 ms
* 999 ms
* 1 second
* 1 minute
* 1 hour
* Large elapsed values

RING BUFFER

* Empty state
* Push and pop
* Full state
* Overflow detection
* Wraparound

Use fake implementations of:

ILed
IUart
IClock
ITimer

Aim for high coverage.

====================================================================
11. DOCUMENTATION
====================================================================

Create comprehensive documentation.

README.md must include:

* Project objective
* Stopwatch functionality
* Supported board
* Features
* Architecture overview
* Directory structure
* Hardware connections
* UART commands
* Timer configuration
* LED behavior
* Build instructions
* Flash instructions
* Debug instructions
* Example terminal sessions

Architecture Document must include Mermaid diagrams for:

* Module architecture
* UART receive flow
* Timer interrupt flow
* Stopwatch state machine
* Command processing sequence
* Build and test flow

Protocol Document must describe:

* UART configuration
* Commands
* ACK format
* NACK format
* STATUS format
* Error codes
* Example terminal session

====================================================================
12. BUILD SYSTEM
====================================================================

Provide the following workflows:

HOST TESTS

cmake --preset host-debug
cmake --build --preset host-debug
ctest --preset host-debug --output-on-failure

TARGET FIRMWARE

cmake --preset stm32-debug
cmake --build --preset stm32-debug

Expected target artifacts:

.elf
.bin
.hex
.map

Display firmware size after linking.

MAKE WRAPPER

make help
make test
make firmware
make clean
make format
make analyze
make docs
make flash
make debug

====================================================================
13. STATIC ANALYSIS
====================================================================

Configure:

* clang-format
* clang-tidy
* cppcheck
* Compiler warnings
* Linker map generation
* Firmware size reporting

Recommended warnings:

-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
-Wundef
-Wdouble-promotion
-Wformat=2
-Werror

====================================================================
14. RUNTIME ERROR HANDLING
====================================================================

Implement handling for:

* HAL initialization failure
* UART initialization failure
* Timer initialization failure
* UART RX overflow
* Command buffer overflow
* Unsupported commands
* Invalid command transitions
* UART framing/noise/overrun errors
* Internal formatting failure

Use explicit error codes.

Do not silently ignore errors.

Provide a visible LED error pattern for fatal initialization failures.

====================================================================
15. EXECUTION PROCEDURE
====================================================================

Follow this sequence:

1. Inspect compiler, CMake, Ninja, Make, Python, OpenOCD and ARM toolchain.
2. Create project directory.
3. Write implementation plan.
4. Generate all project-owned files.
5. Configure host build.
6. Compile host tests.
7. Run all host tests.
8. Fix every test failure.
9. Run formatting checks.
10. Run static analysis.
11. Attempt ARM firmware build.
12. Report missing dependencies if build fails.
13. Never claim success unless commands were executed.
14. Record outputs in docs/verification_report.md.
15. Produce final summary.

====================================================================
16. ACCEPTANCE CRITERIA
====================================================================

The task is complete only when:

* All application-owned code is present.
* No placeholder code remains.
* Host tests compile.
* Host tests pass.
* Stopwatch timing is non-blocking.
* 1 ms timer interrupt is implemented.
* START command works correctly.
* STOP command works correctly.
* RESET command works correctly.
* RESUME command works correctly.
* STATUS output is correct.
* Time formatting is correct.
* Tick wraparound is tested.
* ACK, NACK and STATUS messages are tested.
* Buffers are statically allocated.
* Hardware dependencies are isolated.
* Documentation matches implementation.
* Project can be opened as an independent repository.
* Actual execution results are reported honestly.

Begin by inspecting the environment and then create the project without asking for confirmation.
```
