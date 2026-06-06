/**
 * @file    command_parser.hpp
 * @brief   Stateless parser for the traffic-controller serial protocol.
 *
 * Translates a NUL-terminated command line into a Command value. The grammar:
 *
 *     R<sec>  G<sec>  Y<sec>   (1..kMaxDurationSec, decimal, no sign)
 *     START   STOP    STATUS    HELP
 *
 * Parsing rules:
 *  - Leading and trailing ASCII whitespace is ignored.
 *  - Keywords are matched case-insensitively.
 *  - Set commands require at least one digit and a value within range.
 *  - Anything else yields CommandType::Invalid.
 *
 * Pure logic with no I/O: the parser is a free function over a buffer, making
 * it exhaustively unit-testable on the host. No allocation, no exceptions.
 */
#ifndef APP_COMMAND_PARSER_HPP
#define APP_COMMAND_PARSER_HPP

#include "command.hpp"

#include <cstddef>
#include <cstdint>

namespace app {

class CommandParser {
public:
    /// Smallest accepted phase duration (seconds).
    static constexpr std::uint32_t kMinDurationSec = 1;
    /// Largest accepted phase duration (seconds); guards against overflow when
    /// converted to milliseconds (kMaxDurationSec * 1000 fits in uint32).
    static constexpr std::uint32_t kMaxDurationSec = 3600;

    /**
     * @brief Parse a command line.
     * @param line  NUL-terminated input (may contain surrounding whitespace).
     * @param len   length of @p line excluding the NUL.
     * @return parsed Command; type == Invalid on any malformed input.
     */
    static Command parse(const char* line, std::size_t len) noexcept;

private:
    /// Case-insensitive comparison of [begin,end) against a NUL-terminated word.
    static bool equalsIgnoreCase(const char* begin, const char* end,
                                 const char* word) noexcept;

    /// Parse [begin,end) as a bounded decimal into @p out. False on any error.
    static bool parseDuration(const char* begin, const char* end,
                              std::uint32_t& out) noexcept;
};

} // namespace app

#endif // APP_COMMAND_PARSER_HPP
