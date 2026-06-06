/**
 * @file    main.cpp
 * @brief   Smart Traffic Light Controller entry point (STM32F407G-DISC1).
 *
 * Composition root: this is the only file that knows about both the concrete
 * STM32 drivers and the platform-independent application. It wires the drivers
 * to the application through the hal interfaces and then runs a single
 * non-blocking super-loop:
 *
 *   1. Pull any buffered RX bytes and assemble command lines.
 *   2. Dispatch completed lines to the CommandProcessor (parse + respond).
 *   3. Tick the traffic-light state machine (timer-driven transitions).
 *
 * No blocking delays; all timing flows from the SysTick millisecond clock.
 */
#include "board.hpp"
#include "gpio_led.hpp"
#include "stm32f407_gpio.hpp"
#include "stm32f407_rcc.hpp"
#include "stm32f407_uart.hpp"
#include "systick_clock.hpp"
#include "uart_console.hpp"

#include "command_processor.hpp"
#include "line_assembler.hpp"
#include "soft_timer.hpp"
#include "traffic_light.hpp"

using namespace stm32f407;

namespace {

/// Longest accepted command line (e.g. "STATUS"); generous headroom.
constexpr std::size_t kMaxLine = 16;

/// Configure one GPIOD pin as a push-pull output for an LED.
void configureLedPin(const Gpio& gpiod, std::uint8_t pin) noexcept {
    PinConfig led{};
    led.mode  = PinMode::Output;
    led.otype = OutputType::PushPull;
    led.speed = OutputSpeed::Low;
    led.pull  = Pull::None;
    gpiod.configure(pin, led);
}

} // namespace

int main() {
    // --- Clocks ----------------------------------------------------------
    Rcc rcc(RCC());
    rcc.enableGpioPortD(); // LEDs
    rcc.enableGpioPortA(); // USART2 pins
    rcc.enableUsart2();

    // --- LED GPIOs -------------------------------------------------------
    Gpio gpiod(GPIOD());
    configureLedPin(gpiod, board::kLedRed);
    configureLedPin(gpiod, board::kLedYellow);
    configureLedPin(gpiod, board::kLedGreen);
    configureLedPin(gpiod, board::kLedStatus);

    GpioLed red_led(gpiod, board::kLedRed);
    GpioLed yellow_led(gpiod, board::kLedYellow);
    GpioLed green_led(gpiod, board::kLedGreen);
    GpioLed status_led(gpiod, board::kLedStatus);

    // --- USART2 console (AF7 on PA2/PA3) ---------------------------------
    Gpio gpioa(GPIOA());
    PinConfig uart_pin{};
    uart_pin.mode  = PinMode::Alternate;
    uart_pin.otype = OutputType::PushPull;
    uart_pin.speed = OutputSpeed::VeryHigh;
    uart_pin.pull  = Pull::Up;
    uart_pin.af    = board::kUsart2Af;
    gpioa.configure(board::kUsart2TxPin, uart_pin);
    gpioa.configure(board::kUsart2RxPin, uart_pin);

    Uart uart(USART2());
    UartConfig ucfg{};
    ucfg.baud_rate  = 115200;
    ucfg.periph_clk = board::kApb1Clock;
    uart.init(ucfg);

    UartConsole console(uart);
    uart.setRxInterrupt(true);     // enable RXNE interrupt in the USART
    attachUsart2Isr(&console);     // route USART2_IRQHandler -> console + NVIC

    // --- Millisecond clock (SysTick) -------------------------------------
    SysTickClock clock;
    clock.init(board::kCoreClock);

    // --- Application -----------------------------------------------------
    app::SoftTimer    phase_timer(clock);
    app::TrafficLight light(red_led, yellow_led, green_led, status_led, phase_timer);
    app::CommandProcessor processor(light, console);
    app::LineAssembler<kMaxLine> assembler;

    processor.printBanner();
    processor.printStatus();

    // --- Non-blocking super-loop -----------------------------------------
    while (true) {
        std::uint8_t byte = 0;
        while (console.read(byte)) {
            const app::LineStatus st = assembler.feed(byte);
            if (st == app::LineStatus::Ready) {
                processor.handleLine(assembler.line(), assembler.length());
            } else if (st == app::LineStatus::Overflow) {
                console.writeString("NACK\r\n"); // line too long
            }
        }

        light.update(); // timer-driven phase transitions
    }

    return 0; // never reached
}
