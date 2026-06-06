/**
 * @file    command.hpp
 * @brief   Parsed-command value type shared by the parser and processor.
 */
#ifndef APP_COMMAND_HPP
#define APP_COMMAND_HPP

#include <cstdint>

namespace app {

/// Kind of command recognised on the serial console.
enum class CommandType : std::uint8_t {
    SetRed,    ///< R<sec> : set red phase duration
    SetGreen,  ///< G<sec> : set green phase duration
    SetYellow, ///< Y<sec> : set yellow phase duration
    Start,     ///< START  : begin cycling
    Stop,      ///< STOP   : halt and go IDLE
    Status,    ///< STATUS : report timings and active state
    Help,      ///< HELP   : list supported commands
    Invalid,   ///< unrecognised or malformed input
};

/// A fully parsed command. @ref value is meaningful only for the Set* types.
struct Command {
    CommandType   type  = CommandType::Invalid;
    std::uint32_t value = 0; ///< duration in seconds for Set* commands
};

} // namespace app

#endif // APP_COMMAND_HPP
