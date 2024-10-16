#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Die Deutschen");
MODULE_DESCRIPTION("encoder with gpio interrupt");

#define GPIO_IN_20 532 // PIN 20
#define GPIO_IN_22 534 // PIN 22

#define  DEVICE_MAJOR 0         ///< Requested device node major number or 0 for dynamic allocation
#define  DEVICE_NAME "encoder" 

static int   majorNumber;        ///< Stores the device number -- determined automatically
static int   numberOpens = 0;    ///< Counts the number of times the device is opened
static char *name = "DD";

module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
unsigned int ui_EncoderPos;
int bDirection;

static int     encoder_open(struct inode *, struct file *);
static int     encoder_release(struct inode *, struct file *);
static ssize_t encoder_read(struct file *, char *, size_t, loff_t *);


static struct file_operations fops =
{
   .read = encoder_read,
    .open = encoder_open,
   .release = encoder_release,
};

static int encoder_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "encoder: Device successfully closed\n");
   return 0;
}

static int encoder_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "encoder: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
static ssize_t encoder_read(struct file *filp, char *buffer, size_t length, loff_t *offset){
   printk(KERN_INFO "encode: Encoder position: %d", ui_EncoderPos);
   return ui_EncoderPos;
}


static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    bDirection = gpio_get_value(GPIO_IN_22);
    printk("gpio_irq: Interrupt was triggered and ISR was called!\n");

    if(bDirection == true)
    {
        /* negative direction */ 
        ui_EncoderPos--;
    }
    else
    {
        /* positive direction */ 
        ui_EncoderPos++;
    }
    return IRQ_HANDLED;
}

/**
* @brief This function is called, when the module is loaded into the kernel
*/
static int __init ModuleInit(void) 
{
    // INIT READ
    printk(KERN_INFO "encoder: Hello %s from the RPi Encoder!\n", name);

    majorNumber = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);

    if (majorNumber<0){
        printk(KERN_ALERT "encoder failed to register a major number\n");
        return majorNumber;
    }

    printk(KERN_INFO "encoder: Device registered correctly with major number %d\n", majorNumber);

    printk("qpio_irq: Loading module... ");



    /* Setup GPIO 20 */
    if(gpio_request(GPIO_IN_20, "rpi-gpio-20")) 
    { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_IN_20);
        return -1;
    }
    if(gpio_direction_input(GPIO_IN_20)) 
    {
        printk("Error!\nCan not set GPIO %d to input!\n", GPIO_IN_20);
        gpio_free(GPIO_IN_20);
        return -1;
    }

    /* Setup GPIO 22 */
    if(gpio_request(GPIO_IN_22, "rpi-gpio-20")) 
    { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_IN_22);
        return -1;
    }
    if(gpio_direction_input(GPIO_IN_22)) 
    {
        printk("Error!\nCan not set GPIO %d to input!\n", GPIO_IN_22);
        gpio_free(GPIO_IN_22);
        return -1;
    }
    


    /* Setup the interrupt */
    irq_number = gpio_to_irq(GPIO_IN_20);

    if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0)
    {
        printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
        gpio_free(GPIO_IN_20);

        return -1;
    }

    printk("Done!\n");
    printk("GPIO %d is mapped to IRQ Nr.: %d\n", GPIO_IN_20, irq_number);
    return 0;
}

/**

 * @brief This function is called, when the module is removed from the kernel

 */

static void __exit ModuleExit(void) 
{
    printk("gpio_irq: Unloading module... ");
    free_irq(irq_number, NULL);
    gpio_free(GPIO_IN_20);
    gpio_free(GPIO_IN_22);

    // RELEASE READ
    unregister_chrdev(majorNumber, DEVICE_NAME);
}


module_init(ModuleInit);
module_exit(ModuleExit);