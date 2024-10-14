
#include <gpiod.h> // All of the GPIO stuff...
#include <unistd.h> // usleep()
#include <iostream>

int main() {
    int ret;

    int state = 0;

    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    struct gpiod_line *pin_out;
    struct gpiod_line *pin_in;
    chip = gpiod_chip_open_by_name(chipname);
    pin_out = gpiod_chip_get_line(chip, 17);
    pin_in = gpiod_chip_get_line(chip, 20);

    gpiod_line_request_output(pin_out, "event", 0);

    struct gpiod_line_event event;

    if (!chip) {
        perror("Open chip failed\n");
        return -1;
    }

    if (!pin_out) {
        perror("Get line Out failed\n");
        ret = -1;
        goto close_chip;
    }
    if (!pin_in) {
        perror("Get line Out failed\n");
        ret = -1;
        goto close_chip;
    }

    ret = gpiod_line_request_rising_edge_events(pin_in, "event");
    
    if (ret < 0) {
        perror("Request event notification failed\n");
        ret = -1;
        goto release_line;
    }

    // init state
    gpiod_line_set_value(pin_out, state); 

    while (1) {
        std::cout << "waiting for event" << std::endl;
        gpiod_line_event_wait(pin_in, NULL); /* blocking */
        gpiod_line_set_value(pin_out, 1); 
          
    }

    release_line:
    gpiod_line_release(pin_in);

    close_chip:
    gpiod_chip_close(chip);

    return ret;


}
// $ g++ -o gpioevent gpio_event_new.cpp -lgpiod
// $ ./gpioevent

