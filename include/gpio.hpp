#ifndef GPIO_HPP
#define GPIO_HPP

#include <cstdint>

namespace stm32f407 {

// GPIO Port definitions
enum class GPIOPort : uint8_t {
    GPIOA = 0,
    GPIOB = 1,
    GPIOC = 2,
    GPIOD = 3,
    GPIOE = 4,
    GPIOF = 5,
    GPIOG = 6,
    GPIOH = 7,
    GPIOI = 8
};

// GPIO Pin definitions (0-15)
enum class GPIOPin : uint8_t {
    PIN0 = 0,
    PIN1 = 1,
    PIN2 = 2,
    PIN3 = 3,
    PIN4 = 4,
    PIN5 = 5,
    PIN6 = 6,
    PIN7 = 7,
    PIN8 = 8,
    PIN9 = 9,
    PIN10 = 10,
    PIN11 = 11,
    PIN12 = 12,
    PIN13 = 13,
    PIN14 = 14,
    PIN15 = 15
};

// GPIO Mode
enum class GPIOMode : uint8_t {
    INPUT = 0x00,
    OUTPUT = 0x01,
    ALTERNATE = 0x02,
    ANALOG = 0x03
};

// GPIO Output Type
enum class GPIOOutputType : uint8_t {
    PUSH_PULL = 0,
    OPEN_DRAIN = 1
};

// GPIO Output Speed
enum class GPIOSpeed : uint8_t {
    LOW = 0x00,
    MEDIUM = 0x01,
    FAST = 0x02,
    HIGH = 0x03
};

// GPIO Pull configuration
enum class GPIOPull : uint8_t {
    NO_PULL = 0x00,
    PULL_UP = 0x01,
    PULL_DOWN = 0x02
};

class GPIO {
public:
    GPIO(GPIOPort port);
    
    // Initialize GPIO port and pin
    void init(GPIOPin pin, GPIOMode mode, GPIOSpeed speed = GPIOSpeed::LOW, 
              GPIOPull pull = GPIOPull::NO_PULL, GPIOOutputType type = GPIOOutputType::PUSH_PULL);
    
    // Set alternate function for pin
    void setAlternateFunction(GPIOPin pin, uint8_t afNumber);
    
    // Write pin (output)
    void writePin(GPIOPin pin, bool state);
    
    // Read pin (input)
    bool readPin(GPIOPin pin);
    
    // Toggle pin
    void togglePin(GPIOPin pin);
    
    // Write all pins on port
    void writePort(uint16_t value);
    
    // Read all pins on port
    uint16_t readPort() const;

private:
    GPIOPort m_port;
    volatile uint32_t* m_baseAddress;
    
    volatile uint32_t* getPortBaseAddress(GPIOPort port);
    void enablePortClock();
};

} // namespace stm32f407

#endif // GPIO_HPP
