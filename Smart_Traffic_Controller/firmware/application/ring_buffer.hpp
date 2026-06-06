/**
 * @file    ring_buffer.hpp
 * @brief   Fixed-capacity, statically-allocated SPSC ring buffer.
 *
 * Single-producer / single-consumer FIFO sized at compile time. The intended
 * use is UART RX: the producer is the USART interrupt (push), the consumer is
 * the main loop (pop). With one writer of head_ and one writer of tail_, the
 * structure is safe without locks on a single core as long as the index stores
 * are atomic word writes (true for std::uint32_t on Cortex-M).
 *
 * No dynamic allocation, no exceptions. One slot is intentionally left unused
 * so full and empty are distinguishable without a separate count.
 */
#ifndef APP_RING_BUFFER_HPP
#define APP_RING_BUFFER_HPP

#include <cstddef>
#include <cstdint>

namespace app {

/**
 * @brief Lock-free SPSC ring buffer holding up to (Capacity - 1) elements.
 * @tparam T         element type (trivially copyable)
 * @tparam Capacity  number of slots; usable capacity is Capacity - 1.
 */
template <typename T, std::size_t Capacity>
class RingBuffer {
    static_assert(Capacity >= 2, "RingBuffer needs at least 2 slots");

public:
    /// Maximum number of elements that can be stored at once.
    static constexpr std::size_t kMaxItems = Capacity - 1;

    /// True when no elements are queued.
    bool empty() const noexcept { return head_ == tail_; }

    /// True when no further element can be pushed.
    bool full() const noexcept { return next(head_) == tail_; }

    /// Number of elements currently queued.
    std::size_t size() const noexcept {
        return (head_ + Capacity - tail_) % Capacity;
    }

    /**
     * @brief Append @p value if space remains (producer side).
     * @return true on success, false if the buffer was full (value dropped).
     */
    bool push(const T& value) noexcept {
        const std::size_t n = next(head_);
        if (n == tail_) {
            return false; // full: drop rather than overwrite unread data
        }
        buffer_[head_] = value;
        head_ = n;
        return true;
    }

    /**
     * @brief Remove the oldest element into @p out (consumer side).
     * @return true if an element was dequeued, false if the buffer was empty.
     */
    bool pop(T& out) noexcept {
        if (empty()) {
            return false;
        }
        out = buffer_[tail_];
        tail_ = next(tail_);
        return true;
    }

    /// Discard all queued elements (consumer side).
    void clear() noexcept { tail_ = head_; }

private:
    static std::size_t next(std::size_t i) noexcept { return (i + 1) % Capacity; }

    T buffer_[Capacity]{};
    volatile std::size_t head_ = 0; // written by producer only
    volatile std::size_t tail_ = 0; // written by consumer only
};

} // namespace app

#endif // APP_RING_BUFFER_HPP
