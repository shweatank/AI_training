#include "gpio.hpp"
#include "stm32f407_registers.hpp"
#include <cstdint>

namespace stm32f407 {

GPIO::GPIO(GPIOPort port) : m_port(port) {
    m_baseAddress = getPortBaseAddress(port);
    enablePortClock();
}

volatile uint32_t* GPIO::getPortBaseAddress(GPIOPort port) {
    switch (port) {
        case GPIOPort::GPIOA: return reinterpret_cast<volatile uint32_t*>(GPIOA_BASE);
        case GPIOPort::GPIOB: return reinterpret_cast<volatile uint32_t*>(GPIOB_BASE);
        case GPIOPort::GPIOC: return reinterpret_cast<volatile uint32_t*>(GPIOC_BASE);
        case GPIOPort::GPIOD: return reinterpret_cast<volatile uint32_t*>(GPIOD_BASE);
        case GPIOPort::GPIOE: return reinterpret_cast<volatile uint32_t*>(GPIOE_BASE);
        case GPIOPort::GPIOF: return reinterpret_cast<volatile uint32_t*>(GPIOF_BASE);
        case GPIOPort::GPIOG: return reinterpret_cast<volatile uint32_t*>(GPIOG_BASE);
        case GPIOPort::GPIOH: return reinterpret_cast<volatile uint32_t*>(GPIOH_BASE);
        case GPIOPort::GPIOI: return reinterpret_cast<volatile uint32_t*>(GPIOI_BASE);
        default: return nullptr;
    }
}

void GPIO::enablePortClock() {
    RCCRegisters* rcc = getRCCRegisters();
    
    switch (m_port) {
        case GPIOPort::GPIOA: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; break;
        case GPIOPort::GPIOB: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; break;
        case GPIOPort::GPIOC: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; break;
        case GPIOPort::GPIOD: rcc->AHB1ENR |= RCC_AHB1ENR_GPIODEN; break;
        case GPIOPort::GPIOE: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; break;
        case GPIOPort::GPIOF: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; break;
        case GPIOPort::GPIOG: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; break;
        case GPIOPort::GPIOH: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; break;
        case GPIOPort::GPIOI: rcc->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; break;
        default: break;
    }
}

void GPIO::init(GPIOPin pin, GPIOMode mode, GPIOSpeed speed, GPIOPull pull, GPIOOutputType type) {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return;
    
    uint8_t pinNum = static_cast<uint8_t>(pin);
    
    // Set GPIO mode (MODER register - 2 bits per pin)
    gpio->MODER &= ~(0x3 << (2 * pinNum));
    gpio->MODER |= (static_cast<uint32_t>(mode) << (2 * pinNum));
    
    // Set output type (OTYPER register - 1 bit per pin)
    if (gpio->OTYPER & (1 << pinNum)) {
        gpio->OTYPER &= ~(1 << pinNum);
    }
    gpio->OTYPER |= (static_cast<uint32_t>(type) << pinNum);
    
    // Set speed (OSPEEDR register - 2 bits per pin)
    gpio->OSPEEDR &= ~(0x3 << (2 * pinNum));
    gpio->OSPEEDR |= (static_cast<uint32_t>(speed) << (2 * pinNum));
    
    // Set pull configuration (PUPDR register - 2 bits per pin)
    gpio->PUPDR &= ~(0x3 << (2 * pinNum));
    gpio->PUPDR |= (static_cast<uint32_t>(pull) << (2 * pinNum));
}

void GPIO::setAlternateFunction(GPIOPin pin, uint8_t afNumber) {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return;
    
    uint8_t pinNum = static_cast<uint8_t>(pin);
    
    // Alternate function is split into low (pins 0-7) and high (pins 8-15) registers
    if (pinNum < 8) {
        gpio->AFRL &= ~(0xF << (4 * pinNum));
        gpio->AFRL |= (afNumber << (4 * pinNum));
    } else {
        gpio->AFRH &= ~(0xF << (4 * (pinNum - 8)));
        gpio->AFRH |= (afNumber << (4 * (pinNum - 8)));
    }
}

void GPIO::writePin(GPIOPin pin, bool state) {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return;
    
    uint8_t pinNum = static_cast<uint8_t>(pin);
    
    if (state) {
        // Set bit using BSRR register (lower 16 bits set, upper 16 bits reset)
        gpio->BSRR = (1 << pinNum);
    } else {
        // Reset bit using BSRR register (upper 16 bits reset)
        gpio->BSRR = (1 << (pinNum + 16));
    }
}

bool GPIO::readPin(GPIOPin pin) {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return false;
    
    uint8_t pinNum = static_cast<uint8_t>(pin);
    return (gpio->IDR >> pinNum) & 0x1;
}

void GPIO::togglePin(GPIOPin pin) {
    bool current = readPin(pin);
    writePin(pin, !current);
}

void GPIO::writePort(uint16_t value) {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return;
    
    gpio->ODR = value;
}

uint16_t GPIO::readPort() const {
    GPIORegisters* gpio = getGPIORegisters(reinterpret_cast<uintptr_t>(m_baseAddress));
    
    if (!gpio) return 0;
    
    return static_cast<uint16_t>(gpio->IDR & 0xFFFF);
}

} // namespace stm32f407
