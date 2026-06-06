/**
 * @file    line_assembler.hpp
 * @brief   Assembles received bytes into bounded command lines.
 *
 * Bytes pulled from the UART (in the main loop) are fed in one at a time. A
 * line is completed on CR or LF; the assembler then reports it through a fixed
 * NUL-terminated buffer. Overlong lines (no terminator within the buffer) are
 * flagged as overflow and discarded so a flood of bytes can never corrupt
 * memory or wedge the parser.
 *
 * Statically sized, no allocation, no exceptions. The byte feeding and the
 * line consumption both happen in the main loop, so no concurrency concerns.
 */
#ifndef APP_LINE_ASSEMBLER_HPP
#define APP_LINE_ASSEMBLER_HPP

#include <cstddef>
#include <cstdint>

namespace app {

/// Result of feeding one byte into the assembler.
enum class LineStatus : std::uint8_t {
    Incomplete, ///< byte buffered; line not finished yet
    Ready,      ///< a complete line is available via line()/length()
    Overflow,   ///< line exceeded capacity; it was discarded
};

/**
 * @brief Accumulates bytes into newline-terminated lines.
 * @tparam Capacity  maximum line length in characters (excluding the NUL).
 */
template <std::size_t Capacity>
class LineAssembler {
    static_assert(Capacity >= 1, "LineAssembler needs room for at least 1 char");

public:
    /**
     * @brief Feed one received byte.
     * @return Ready when @p byte terminated a non-empty line, Overflow when the
     *         line grew past Capacity (line discarded), else Incomplete.
     */
    LineStatus feed(std::uint8_t byte) noexcept {
        const char c = static_cast<char>(byte);

        if (c == '\r' || c == '\n') {
            if (overflowed_) {
                reset();
                return LineStatus::Overflow;
            }
            if (len_ == 0) {
                return LineStatus::Incomplete; // ignore blank lines / bare CRLF
            }
            buffer_[len_] = '\0';
            ready_len_    = len_;
            len_          = 0;
            return LineStatus::Ready;
        }

        if (overflowed_) {
            return LineStatus::Incomplete; // swallow until the terminator arrives
        }
        if (len_ >= Capacity) {
            overflowed_ = true; // mark; report Overflow once terminated
            return LineStatus::Incomplete;
        }
        buffer_[len_++] = c;
        return LineStatus::Incomplete;
    }

    /// Pointer to the most recently completed line (valid after Ready).
    const char* line() const noexcept { return buffer_; }

    /// Length of the most recently completed line (excludes NUL).
    std::size_t length() const noexcept { return ready_len_; }

    /// Discard any partially accumulated line.
    void reset() noexcept {
        len_        = 0;
        overflowed_ = false;
    }

private:
    char        buffer_[Capacity + 1]{}; // +1 for NUL terminator
    std::size_t len_        = 0;
    std::size_t ready_len_  = 0;
    bool        overflowed_ = false;
};

} // namespace app

#endif // APP_LINE_ASSEMBLER_HPP
