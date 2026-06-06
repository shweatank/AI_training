/**
 * @file    command_processor.hpp
 * @brief   Applies parsed commands to the state machine and emits responses.
 *
 * Bridges the serial protocol to the TrafficLight. For each completed line it:
 *   1. parses the line (CommandParser),
 *   2. applies the effect to the TrafficLight,
 *   3. writes "ACK" or "NACK", and
 *   4. for state-changing commands, prints the updated STATUS line.
 *
 * All formatting uses fixed buffers (app::fmt); there is no heap, no <cstdio>
 * sprintf, no exceptions. Runs only in the main loop / scheduler context, never
 * in an ISR. Depends solely on hal::IUart and app::TrafficLight, so it is fully
 * host-testable with a capturing fake UART.
 */
#ifndef APP_COMMAND_PROCESSOR_HPP
#define APP_COMMAND_PROCESSOR_HPP

#include "command.hpp"
#include "hal/i_uart.hpp"
#include "traffic_light.hpp"

#include <cstddef>

namespace app {

class CommandProcessor {
public:
    CommandProcessor(TrafficLight& light, hal::IUart& uart) noexcept
        : light_(light), uart_(uart) {}

    /// Parse and execute one NUL-terminated command @p line of length @p len.
    void handleLine(const char* line, std::size_t len) noexcept;

    /// Emit the full STATUS report line.
    void printStatus() noexcept;

    /// Emit the HELP text listing supported commands.
    void printHelp() noexcept;

    /// Emit a one-line banner (used at startup).
    void printBanner() noexcept;

    /// Human-readable name for a state ("IDLE"/"RED"/"GREEN"/"YELLOW").
    static const char* stateName(TrafficState state) noexcept;

private:
    void ack() noexcept;
    void nack() noexcept;

    /// Apply @p cmd to the state machine. Returns true if it was a valid,
    /// state-changing command whose STATUS should be echoed.
    bool apply(const Command& cmd, bool& valid) noexcept;

    TrafficLight& light_;
    hal::IUart&   uart_;
};

} // namespace app

#endif // APP_COMMAND_PROCESSOR_HPP
