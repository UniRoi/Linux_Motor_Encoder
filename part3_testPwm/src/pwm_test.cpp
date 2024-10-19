#include <gpiod.h>
#include <iostream>
#include <unistd.h> // For usleep (microsecond sleep)

// Function to generate software-based PWM signal
void softwarePWM(int pin, int frequency, float duty_cycle, int duration_ms) {
    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int period_us = 1000000 / frequency;  // Period in microseconds
    int high_time = period_us * duty_cycle;
    int low_time = period_us - high_time;
    int iterations = (duration_ms * 1000) / period_us; // Convert duration to microseconds

    // Open GPIO chip
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip\n";
        return;
    }

    // Get GPIO line
    line = gpiod_chip_get_line(chip, pin);
    if (!line) {
        std::cerr << "Failed to get GPIO line\n";
        gpiod_chip_close(chip);
        return;
    }

    // Request GPIO line as output
    if (gpiod_line_request_output(line, "pwm-test", 0) < 0) {
        std::cerr << "Failed to request line as output\n";
        gpiod_chip_close(chip);
        return;
    }

    // Generate PWM signal
    for (int i = 0; i < iterations; i++) {
        gpiod_line_set_value(line, 1);  // Set high
        usleep(high_time);              // High time

        gpiod_line_set_value(line, 0);  // Set low
        usleep(low_time);               // Low time
    }

    // Release the line and close the chip
    gpiod_line_release(line);
    gpiod_chip_close(chip);
}

int main() {
    int gpio_pin = 18;          // PWM Pin (GPIO18 / Pin 12)
    int frequency = 1000;       // 1kHz
    float duty_cycle = 0.5;     // 50% duty cycle
    int duration = 5000;        // Run for 5000 ms (5 seconds)

    std::cout << "Generating software-based PWM...\n";
    softwarePWM(gpio_pin, frequency, duty_cycle, duration);
    std::cout << "PWM finished.\n";

    return 0;
}
