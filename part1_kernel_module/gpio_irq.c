#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM for a gpio interrupt");

#define GPIO_17 17 // according to /sys/kernel/debug/gpio
#define GPIO_OUT 17
#define GPIO_IN 20

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    static int value = 1;
    printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
    gpio_set_value(GPIO_OUT, !value);
    value = !value;
    return IRQ_HANDLED;
}

/**
* @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) 
{
    printk("qpio_irq: Loading module... ");

    /* Setup the gpio's*/
    if(gpio_request(GPIO_IN, "rpi-gpio-20")) 
    { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_IN);
        return -1;
    }

    if(gpio_request(GPIO_OUT, "rpi-gpio-17")) 
    { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_OUT);
        return -1;
    }

    /* Set GPIO 20 direction */
    if(gpio_direction_input(GPIO_IN)) 
    {
        printk("Error!\nCan not set GPIO %d to input!\n", GPIO_IN);
        gpio_free(GPIO_IN);
        return -1;
    }

    /* Set GPIO 17 direction */
    if(gpio_direction_output(GPIO_OUT, 0)) 
    {
        printk("Error!\nCan not set GPIO %d to input!\n", GPIO_OUT);
        gpio_free(GPIO_OUT);
        return -1;
    }

    /* Setup the interrupt */
    irq_number = gpio_to_irq(GPIO_IN);

    if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0)
    {
        printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
        gpio_free(GPIO_IN);

        return -1;
    }

    printk("Done!\n");
    printk("GPIO %d is mapped to IRQ Nr.: %d\n", GPIO_IN, irq_number);

    return 0;
}

/**

 * @brief This function is called, when the module is removed from the kernel

 */

static void __exit ModuleExit(void) 
{
    printk("gpio_irq: Unloading module... ");
    free_irq(irq_number, NULL);
    gpio_free(GPIO_IN);
    gpio_free(GPIO_OUT);
}

module_init(ModuleInit);
module_exit(ModuleExit);