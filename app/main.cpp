#include "gpio.hpp"
#include "uart.hpp"

using namespace stm32f407;

// Simple delay function (milliseconds)
void delay_ms(uint32_t ms) {
    volatile uint32_t count = ms * 168;  // Approximate for 168MHz clock
    while (count--);
}

int main() {
    // Initialize UART2 at 115200 baud (typical for serial communication)
    UART uart(UARTPort::UART2);
    uart.init(115200, UARTDataBits::BITS_8, UARTStopBits::STOP_BITS_1, UARTParity::NO_PARITY);
    
    // Send startup message
    uart.sendString("\r\n========================================\r\n");
    uart.sendString("STM32F407 GPIO and UART Driver Demo\r\n");
    uart.sendString("========================================\r\n\r\n");
    
    // Initialize GPIO LED (example: PD12, PD13, PD14, PD15 on STM32F4-Discovery)
    GPIO gpio_d(GPIOPort::GPIOD);
    
    // Initialize LEDs as outputs
    gpio_d.init(GPIOPin::PIN12, GPIOMode::OUTPUT, GPIOSpeed::MEDIUM);
    gpio_d.init(GPIOPin::PIN13, GPIOMode::OUTPUT, GPIOSpeed::MEDIUM);
    gpio_d.init(GPIOPin::PIN14, GPIOMode::OUTPUT, GPIOSpeed::MEDIUM);
    gpio_d.init(GPIOPin::PIN15, GPIOMode::OUTPUT, GPIOSpeed::MEDIUM);
    
    uart.sendString("LEDs initialized on GPIOD (pins 12-15)\r\n\r\n");
    
    // Initialize a button on PA0 (user button on Discovery board)
    GPIO gpio_a(GPIOPort::GPIOA);
    gpio_a.init(GPIOPin::PIN0, GPIOMode::INPUT, GPIOSpeed::LOW, GPIOPull::NO_PULL);
    
    uart.sendString("Button initialized on GPIOA pin 0\r\n\r\n");
    
    // ========== Main Application Loop ==========
    uint32_t counter = 0;
    bool led_state = false;
    
    uart.sendString("Entering main loop...\r\n");
    uart.sendString("Press button to toggle LEDs\r\n\r\n");
    
    while (true) {
        // Check button state
        if (gpio_a.readPin(GPIOPin::PIN0)) {
            // Button pressed - toggle LED state
            led_state = !led_state;
            
            // Write LED state
            if (led_state) {
                uart.sendString("Button pressed - LEDs ON\r\n");
                gpio_d.writePin(GPIOPin::PIN12, true);
                gpio_d.writePin(GPIOPin::PIN13, true);
                gpio_d.writePin(GPIOPin::PIN14, true);
                gpio_d.writePin(GPIOPin::PIN15, true);
            } else {
                uart.sendString("Button pressed - LEDs OFF\r\n");
                gpio_d.writePin(GPIOPin::PIN12, false);
                gpio_d.writePin(GPIOPin::PIN13, false);
                gpio_d.writePin(GPIOPin::PIN14, false);
                gpio_d.writePin(GPIOPin::PIN15, false);
            }
            
            delay_ms(500);  // Debounce delay
        }
        
        // Check for UART input
        if (uart.isDataAvailable()) {
            uint8_t received_byte = uart.receiveByte();
            
            // Echo the received byte
            uart.sendByte(received_byte);
            
            // Perform action based on received character
            switch (received_byte) {
                case '1':
                    uart.sendString("\r\nToggling LED D12\r\n");
                    gpio_d.togglePin(GPIOPin::PIN12);
                    break;
                case '2':
                    uart.sendString("\r\nToggling LED D13\r\n");
                    gpio_d.togglePin(GPIOPin::PIN13);
                    break;
                case '3':
                    uart.sendString("\r\nToggling LED D14\r\n");
                    gpio_d.togglePin(GPIOPin::PIN14);
                    break;
                case '4':
                    uart.sendString("\r\nToggling LED D15\r\n");
                    gpio_d.togglePin(GPIOPin::PIN15);
                    break;
                case 'A':
                case 'a':
                    uart.sendString("\r\nTurning ALL LEDs ON\r\n");
                    gpio_d.writePin(GPIOPin::PIN12, true);
                    gpio_d.writePin(GPIOPin::PIN13, true);
                    gpio_d.writePin(GPIOPin::PIN14, true);
                    gpio_d.writePin(GPIOPin::PIN15, true);
                    break;
                case 'O':
                case 'o':
                    uart.sendString("\r\nTurning ALL LEDs OFF\r\n");
                    gpio_d.writePin(GPIOPin::PIN12, false);
                    gpio_d.writePin(GPIOPin::PIN13, false);
                    gpio_d.writePin(GPIOPin::PIN14, false);
                    gpio_d.writePin(GPIOPin::PIN15, false);
                    break;
                case 'H':
                case 'h':
                    uart.sendString("\r\n\r\nCommands:\r\n");
                    uart.sendString("  1-4: Toggle LED D12-D15\r\n");
                    uart.sendString("  A:   All LEDs ON\r\n");
                    uart.sendString("  O:   All LEDs OFF\r\n");
                    uart.sendString("  S:   Status\r\n");
                    uart.sendString("  H:   Help\r\n\r\n");
                    break;
                case 'S':
                case 's':
                    uart.sendString("\r\nStatus:\r\n");
                    uart.sendString("  Loop Counter: ");
                    // Simple number to ASCII conversion
                    {
                        char buffer[12];
                        uint32_t temp = counter;
                        int i = 0;
                        if (temp == 0) {
                            buffer[i++] = '0';
                        } else {
                            while (temp > 0) {
                                buffer[i++] = '0' + (temp % 10);
                                temp /= 10;
                            }
                            // Reverse the string
                            for (int j = 0; j < i / 2; j++) {
                                char tmp = buffer[j];
                                buffer[j] = buffer[i - 1 - j];
                                buffer[i - 1 - j] = tmp;
                            }
                        }
                        buffer[i] = '\0';
                        uart.sendString(buffer);
                    }
                    uart.sendString("\r\n");
                    break;
                default:
                    uart.sendString("\r\nUnknown command. Type 'H' for help.\r\n");
                    break;
            }
        }
        
        // Increment counter
        counter++;
        
        // Small delay to prevent CPU overload
        delay_ms(10);
    }
    
    return 0;
}
