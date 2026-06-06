/**
 * @file    mocks.hpp
 * @brief   Host-side fakes implementing the hal interfaces for unit tests.
 *
 * These let the platform-independent application be exercised with no hardware:
 *  - FakeClock  : a settable/advanceable millisecond clock.
 *  - FakeLed    : records its on/off state.
 *  - FakeUart   : captures transmitted bytes as a string and feeds queued RX.
 */
#ifndef TESTS_MOCKS_HPP
#define TESTS_MOCKS_HPP

#include "hal/i_clock.hpp"
#include "hal/i_led.hpp"
#include "hal/i_uart.hpp"

#include <cstdint>
#include <deque>
#include <string>

namespace mock {

/// Deterministic, manually-driven millisecond clock.
class FakeClock final : public hal::IClock {
public:
    std::uint32_t nowMs() const noexcept override { return now_; }

    void setNow(std::uint32_t ms) noexcept { now_ = ms; }
    void advance(std::uint32_t ms) noexcept { now_ += ms; }

private:
    std::uint32_t now_ = 0;
};

/// LED that simply remembers whether it is on.
class FakeLed final : public hal::ILed {
public:
    void on() noexcept override { on_ = true; }
    void off() noexcept override { on_ = false; }
    void set(bool illuminated) noexcept override { on_ = illuminated; }
    bool isOn() const noexcept override { return on_; }

private:
    bool on_ = false;
};

/// UART that captures TX into a string and serves queued RX bytes.
class FakeUart final : public hal::IUart {
public:
    void write(std::uint8_t byte) noexcept override {
        tx_.push_back(static_cast<char>(byte));
    }

    void writeString(const char* str) noexcept override {
        if (str == nullptr) return;
        tx_ += str;
    }

    bool read(std::uint8_t& out) noexcept override {
        if (rx_.empty()) return false;
        out = rx_.front();
        rx_.pop_front();
        return true;
    }

    // --- Test helpers ---
    const std::string& tx() const noexcept { return tx_; }
    void clearTx() noexcept { tx_.clear(); }
    bool txContains(const std::string& needle) const {
        return tx_.find(needle) != std::string::npos;
    }
    void queueRx(const std::string& bytes) {
        for (char c : bytes) rx_.push_back(static_cast<std::uint8_t>(c));
    }

private:
    std::string                tx_;
    std::deque<std::uint8_t>   rx_;
};

} // namespace mock

#endif // TESTS_MOCKS_HPP
