#include <linux/interrupt.h>
#include <gpiod.h> // All of the GPIO stuff...
#include <unistd.h> // usleep()
#include <iostream>

int state = 0;

static irq_handler_t my_gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    state = 1;
}

int main() {
    int gpio_to_irq(unsigned gpio);
    result = request_irq(irq, // the interrupt number
    (irq_handler_t) my_gpio_irq_handler, // pointer to the handler
    IRQF_TRIGGER_RISING, // interrupt on rising edge
    "my_gpio_handler", // used to identify the owner
    NULL); // *dev_id for shared interrupt lines, NULL


    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    struct gpiod_line *pin_out;
    struct gpiod_line *pin_in;
    chip = gpiod_chip_open_by_name(chipname);
    pin_out = gpiod_chip_get_line(chip, 17);
    pin_in = gpiod_chip_get_line(chip, 20);

    gpiod_line_request_output(pin_out, "polling", 0);
    gpiod_line_request_input(pin_in, "polling");

    for (int counter=0;counter<1000;counter++)
    {
        gpiod_line_set_value(pin_out, state);           
    }

    free_irq(irq, NULL); // Free the IRQ number, no *dev_id required in this case

    gpiod_line_set_value(pin_out, 0);
    gpiod_line_release(pin_out);
    gpiod_line_release(pin_in);
    gpiod_chip_close(chip);

    return 0;
}