/**
 * @file    command_parser.cpp
 * @brief   Implementation of the traffic-controller command parser.
 */
#include "command_parser.hpp"

namespace app {

namespace {

bool isSpace(char c) noexcept {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}

char toUpper(char c) noexcept {
    return (c >= 'a' && c <= 'z') ? static_cast<char>(c - ('a' - 'A')) : c;
}

bool isDigit(char c) noexcept {
    return c >= '0' && c <= '9';
}

} // namespace

bool CommandParser::equalsIgnoreCase(const char* begin, const char* end,
                                     const char* word) noexcept {
    for (const char* p = begin; p < end; ++p, ++word) {
        if (*word == '\0') {
            return false; // input longer than the keyword
        }
        if (toUpper(*p) != toUpper(*word)) {
            return false;
        }
    }
    return *word == '\0'; // both ended together
}

bool CommandParser::parseDuration(const char* begin, const char* end,
                                  std::uint32_t& out) noexcept {
    if (begin >= end) {
        return false; // no digits at all
    }
    std::uint32_t value = 0;
    for (const char* p = begin; p < end; ++p) {
        if (!isDigit(*p)) {
            return false;
        }
        const std::uint32_t digit = static_cast<std::uint32_t>(*p - '0');
        // Reject anything that would exceed the accepted maximum (also blocks
        // arithmetic overflow since kMaxDurationSec is far below UINT32_MAX).
        if (value > (kMaxDurationSec / 10U)) {
            return false;
        }
        value = value * 10U + digit;
        if (value > kMaxDurationSec) {
            return false;
        }
    }
    if (value < kMinDurationSec) {
        return false;
    }
    out = value;
    return true;
}

Command CommandParser::parse(const char* line, std::size_t len) noexcept {
    Command cmd{}; // defaults to Invalid

    if (line == nullptr) {
        return cmd;
    }

    // Trim surrounding whitespace -> working range [begin, end).
    const char* begin = line;
    const char* end   = line + len;
    while (begin < end && isSpace(*begin)) {
        ++begin;
    }
    while (end > begin && isSpace(*(end - 1))) {
        --end;
    }
    if (begin == end) {
        return cmd; // empty after trimming
    }

    // Keyword commands first.
    if (equalsIgnoreCase(begin, end, "START"))  { cmd.type = CommandType::Start;  return cmd; }
    if (equalsIgnoreCase(begin, end, "STOP"))   { cmd.type = CommandType::Stop;   return cmd; }
    if (equalsIgnoreCase(begin, end, "STATUS")) { cmd.type = CommandType::Status; return cmd; }
    if (equalsIgnoreCase(begin, end, "HELP"))   { cmd.type = CommandType::Help;   return cmd; }

    // Single-letter prefix + decimal duration: R<sec> / G<sec> / Y<sec>.
    CommandType set_type = CommandType::Invalid;
    switch (toUpper(*begin)) {
        case 'R': set_type = CommandType::SetRed;    break;
        case 'G': set_type = CommandType::SetGreen;  break;
        case 'Y': set_type = CommandType::SetYellow; break;
        default:  return cmd; // unknown leading token
    }

    std::uint32_t seconds = 0;
    if (!parseDuration(begin + 1, end, seconds)) {
        return cmd; // malformed or out-of-range duration -> Invalid
    }
    cmd.type  = set_type;
    cmd.value = seconds;
    return cmd;
}

} // namespace app
