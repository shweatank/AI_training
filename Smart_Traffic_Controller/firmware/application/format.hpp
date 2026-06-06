/**
 * @file    format.hpp
 * @brief   Tiny allocation-free integer-to-decimal formatting helper.
 *
 * Used to build STATUS / report strings without <cstdio> sprintf or any heap.
 * Writes into a caller-supplied fixed buffer and always NUL-terminates,
 * returning the number of characters written (excluding the terminator) or 0
 * if the buffer is too small. No exceptions.
 */
#ifndef APP_FORMAT_HPP
#define APP_FORMAT_HPP

#include <cstddef>
#include <cstdint>

namespace app {
namespace fmt {

/**
 * @brief Render @p value as decimal into @p buf.
 * @param buf      destination buffer.
 * @param buf_len  size of @p buf in bytes (must hold digits + NUL).
 * @return characters written excluding the NUL terminator, or 0 on overflow.
 */
inline std::size_t toDecimal(std::uint32_t value, char* buf, std::size_t buf_len) noexcept {
    if (buf == nullptr || buf_len == 0) {
        return 0;
    }

    // Render digits least-significant first into a scratch area.
    char tmp[10]; // 2^32-1 == 4294967295 -> 10 digits
    std::size_t n = 0;
    do {
        tmp[n++] = static_cast<char>('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U && n < sizeof(tmp));

    if (n + 1 > buf_len) {
        buf[0] = '\0';
        return 0; // not enough room for digits + NUL
    }

    // Reverse into the destination.
    for (std::size_t i = 0; i < n; ++i) {
        buf[i] = tmp[n - 1 - i];
    }
    buf[n] = '\0';
    return n;
}

/**
 * @brief Append the NUL-terminated @p src onto @p buf at offset @p pos.
 * @return new write offset, clamped so @p buf stays NUL-terminated within
 *         @p buf_len. Bounds-checked: never writes past the buffer.
 */
inline std::size_t append(char* buf, std::size_t buf_len, std::size_t pos,
                          const char* src) noexcept {
    if (buf == nullptr || src == nullptr || buf_len == 0) {
        return pos;
    }
    while (*src != '\0' && pos + 1 < buf_len) {
        buf[pos++] = *src++;
    }
    buf[pos] = '\0';
    return pos;
}

} // namespace fmt
} // namespace app

#endif // APP_FORMAT_HPP
