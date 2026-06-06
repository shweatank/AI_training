#ifndef UART_HPP
#define UART_HPP

#include <cstdint>
#include "gpio.hpp"

namespace stm32f407 {

// UART Port selection
enum class UARTPort : uint8_t {
    UART1 = 1,
    UART2 = 2,
    UART3 = 3,
    UART4 = 4,
    UART5 = 5,
    UART6 = 6
};

// UART Data bits
enum class UARTDataBits : uint8_t {
    BITS_8 = 0,
    BITS_9 = 1
};

// UART Stop bits
enum class UARTStopBits : uint8_t {
    STOP_BITS_1 = 0,
    STOP_BITS_2 = 2
};

// UART Parity
enum class UARTParity : uint8_t {
    NO_PARITY = 0,
    EVEN_PARITY = 2,
    ODD_PARITY = 3
};

class UART {
public:
    UART(UARTPort port);
    
    // Initialize UART with baud rate
    void init(uint32_t baudRate, UARTDataBits dataBits = UARTDataBits::BITS_8,
              UARTStopBits stopBits = UARTStopBits::STOP_BITS_1,
              UARTParity parity = UARTParity::NO_PARITY);
    
    // Send single byte
    void sendByte(uint8_t byte);
    
    // Receive single byte (blocking)
    uint8_t receiveByte();
    
    // Send string
    void sendString(const char* str);
    
    // Check if data is available
    bool isDataAvailable() const;
    
    // Check if transmitter is busy
    bool isTransmitterBusy() const;
    
    // Enable UART
    void enable();
    
    // Disable UART
    void disable();

private:
    UARTPort m_port;
    volatile uint32_t* m_baseAddress;
    
    volatile uint32_t* getUARTBaseAddress(UARTPort port);
    void enableUARTClock();
    void configurePins();
    uint32_t calculateBaudRateRegister(uint32_t baudRate);
};

} // namespace stm32f407

#endif // UART_HPP
