#include "uart.hpp"
#include "stm32f407_registers.hpp"
#include <cstdint>

namespace stm32f407 {

UART::UART(UARTPort port) : m_port(port) {
    m_baseAddress = getUARTBaseAddress(port);
    enableUARTClock();
}

volatile uint32_t* UART::getUARTBaseAddress(UARTPort port) {
    switch (port) {
        case UARTPort::UART1: return reinterpret_cast<volatile uint32_t*>(USART1_BASE);
        case UARTPort::UART2: return reinterpret_cast<volatile uint32_t*>(USART2_BASE);
        case UARTPort::UART3: return reinterpret_cast<volatile uint32_t*>(USART3_BASE);
        case UARTPort::UART4: return reinterpret_cast<volatile uint32_t*>(UART4_BASE);
        case UARTPort::UART5: return reinterpret_cast<volatile uint32_t*>(UART5_BASE);
        case UARTPort::UART6: return reinterpret_cast<volatile uint32_t*>(USART6_BASE);
        default: return nullptr;
    }
}

void UART::enableUARTClock() {
    RCCRegisters* rcc = getRCCRegisters();
    
    switch (m_port) {
        case UARTPort::UART1: rcc->APB2ENR |= RCC_APB2ENR_USART1EN; break;
        case UARTPort::UART2: rcc->APB1ENR |= RCC_APB1ENR_USART2EN; break;
        case UARTPort::UART3: rcc->APB1ENR |= RCC_APB1ENR_USART3EN; break;
        case UARTPort::UART4: rcc->APB1ENR |= RCC_APB1ENR_UART4EN; break;
        case UARTPort::UART5: rcc->APB1ENR |= RCC_APB1ENR_UART5EN; break;
        case UARTPort::UART6: rcc->APB2ENR |= RCC_APB2ENR_USART6EN; break;
        default: break;
    }
}

void UART::configurePins() {
    // Configure TX and RX pins based on UART port
    // This is a simplified example - actual pin mapping depends on your board layout
    
    switch (m_port) {
        case UARTPort::UART1: {
            // UART1: TX=PA9 (AF7), RX=PA10 (AF7)
            GPIO gpio(GPIOPort::GPIOA);
            gpio.init(GPIOPin::PIN9, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN9, 7);
            gpio.init(GPIOPin::PIN10, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN10, 7);
            break;
        }
        case UARTPort::UART2: {
            // UART2: TX=PA2 (AF7), RX=PA3 (AF7)
            GPIO gpio(GPIOPort::GPIOA);
            gpio.init(GPIOPin::PIN2, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN2, 7);
            gpio.init(GPIOPin::PIN3, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN3, 7);
            break;
        }
        case UARTPort::UART3: {
            // UART3: TX=PB10 (AF7), RX=PB11 (AF7)
            GPIO gpio(GPIOPort::GPIOB);
            gpio.init(GPIOPin::PIN10, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN10, 7);
            gpio.init(GPIOPin::PIN11, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN11, 7);
            break;
        }
        case UARTPort::UART4: {
            // UART4: TX=PA0 (AF8), RX=PA1 (AF8)
            GPIO gpio(GPIOPort::GPIOA);
            gpio.init(GPIOPin::PIN0, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN0, 8);
            gpio.init(GPIOPin::PIN1, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN1, 8);
            break;
        }
        case UARTPort::UART5: {
            // UART5: TX=PC12 (AF8), RX=PD2 (AF8)
            GPIO gpioc(GPIOPort::GPIOC);
            gpioc.init(GPIOPin::PIN12, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpioc.setAlternateFunction(GPIOPin::PIN12, 8);
            GPIO gpiod(GPIOPort::GPIOD);
            gpiod.init(GPIOPin::PIN2, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpiod.setAlternateFunction(GPIOPin::PIN2, 8);
            break;
        }
        case UARTPort::UART6: {
            // UART6: TX=PC6 (AF8), RX=PC7 (AF8)
            GPIO gpio(GPIOPort::GPIOC);
            gpio.init(GPIOPin::PIN6, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN6, 8);
            gpio.init(GPIOPin::PIN7, GPIOMode::ALTERNATE, GPIOSpeed::HIGH);
            gpio.setAlternateFunction(GPIOPin::PIN7, 8);
            break;
        }
        default: break;
    }
}

uint32_t UART::calculateBaudRateRegister(uint32_t baudRate) {
    // APB1 clock = 42 MHz, APB2 clock = 84 MHz
    uint32_t clock = (m_port == UARTPort::UART1 || m_port == UARTPort::UART6) ? 84000000 : 42000000;
    
    // BRR = clock / (16 * baudRate)
    return clock / (16 * baudRate);
}

void UART::init(uint32_t baudRate, UARTDataBits dataBits, UARTStopBits stopBits, UARTParity parity) {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!uart) return;
    
    // Configure pins
    configurePins();
    
    // Disable UART
    uart->CR1 &= ~UART_CR1_UE;
    
    // Set baud rate
    uart->BRR = calculateBaudRateRegister(baudRate);
    
    // Configure CR1
    uint32_t cr1 = 0;
    cr1 |= (static_cast<uint32_t>(dataBits) << 12);     // M bit for word length
    cr1 |= (static_cast<uint32_t>(parity) << 9);        // Parity bits
    if (parity != UARTParity::NO_PARITY) {
        cr1 |= UART_CR1_PCE;                            // Enable parity control
    }
    cr1 |= UART_CR1_TE | UART_CR1_RE;                   // Enable TX and RX
    cr1 |= UART_CR1_UE;                                 // Enable UART
    
    uart->CR1 = cr1;
    
    // Configure CR2 for stop bits
    uart->CR2 &= ~(0x3 << 12);
    uart->CR2 |= (static_cast<uint32_t>(stopBits) << 12);
    
    // CR3 - no flow control in this basic implementation
    uart->CR3 = 0;
}

void UART::sendByte(uint8_t byte) {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!uart) return;
    
    // Wait for transmitter to be ready
    while (!(uart->SR & UART_SR_TXE));
    
    // Send byte
    uart->DR = byte;
}

uint8_t UART::receiveByte() {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!uart) return 0;
    
    // Wait for data to be available
    while (!(uart->SR & UART_SR_RXNE));
    
    return static_cast<uint8_t>(uart->DR & 0xFF);
}

void UART::sendString(const char* str) {
    if (!str) return;
    
    while (*str != '\0') {
        sendByte(*str);
        str++;
    }
}

bool UART::isDataAvailable() const {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!uart) return false;
    
    return (uart->SR & UART_SR_RXNE) != 0;
}

bool UART::isTransmitterBusy() const {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!uart) return true;
    
    return (uart->SR & UART_SR_TC) == 0;
}

void UART::enable() {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (uart) {
        uart->CR1 |= UART_CR1_UE;
    }
}

void UART::disable() {
    UARTRegisters* uart = getUARTRegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (uart) {
        uart->CR1 &= ~UART_CR1_UE;
    }
}

} // namespace stm32f407
