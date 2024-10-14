#include <gpiod.h> // All of the GPIO stuff...
#include <unistd.h> // usleep()
#include <iostream>

int main()
{
    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    struct gpiod_line *pin_out;
    struct gpiod_line *pin_in;
    chip = gpiod_chip_open_by_name(chipname);
    pin_out = gpiod_chip_get_line(chip, 17);
    pin_in = gpiod_chip_get_line(chip, 20);

    gpiod_line_request_output(pin_out, "polling", 0);
    gpiod_line_request_input(pin_in, "polling");

    int state = 0;

    while (true)
    {
        state = 0;
        //usleep(10); // 0.1 seconds   
        if(gpiod_line_get_value(pin_in) == 1) {
            state = 1;
        }
        gpiod_line_set_value(pin_out, state);     
    }
    // for (int counter=0;counter<10000000000;counter++)
    // {
    //     state = 0;
    //     usleep(10); // 0.1 seconds   
    //     if(gpiod_line_get_value(pin_in) == 1) {
    //         state = 1;
    //     }
    //     gpiod_line_set_value(pin_out, state);           
    // }

    gpiod_line_set_value(pin_out, 0);
    gpiod_line_release(pin_out);
    gpiod_line_release(pin_in);
    gpiod_chip_close(chip);
    return 0;
}
// $ g++ -o gpioread gpio_poll_new.cpp -lgpiod
// $ ./gpioread
