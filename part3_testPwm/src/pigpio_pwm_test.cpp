#include <iostream>
#include <pigpio.h>  // Include pigpio header

int main() {
    // Initialize pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed!\n";
        return 1;
    }

    int gpio_pin = 18;   // GPIO 18 (Physical Pin 12)
    int frequency = 1000; // 1kHz PWM
    int duty_cycle = 128; // 50% duty cycle (range: 0-255)

    // Set PWM on GPIO pin
    gpioSetPWMfrequency(gpio_pin, frequency);
    gpioPWM(gpio_pin, duty_cycle);

    std::cout << "PWM started on GPIO " << gpio_pin << " with " << frequency << "Hz and 50% duty cycle\n";

    // Wait 5 seconds
    gpioDelay(5000000);

    // Stop PWM
    gpioPWM(gpio_pin, 0);

    std::cout << "PWM stopped\n";

    // Terminate pigpio library
    gpioTerminate();

    return 0;
}
