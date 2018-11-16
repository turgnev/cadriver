#include <linux/init.h>      
#include <linux/module.h>    
#include <linux/kernel.h>    

#include <linux/interrupt.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");      
MODULE_AUTHOR("Vladislav Turgenev");  
MODULE_DESCRIPTION("Pulse coin acceptor driver"); 
MODULE_VERSION("0.1");     

static int ca_counter = 0;

static int irq_number; 
static unsigned int signal_pin = 18;    // pin 12 

static irq_handler_t signal_pin_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int __init cadriver_init(void) 
{
	int result = 0;

    printk(KERN_INFO "Coin acceptor module is loaded\n");

    gpio_request(signal_pin, "sysfs");       
    gpio_direction_input(signal_pin);        
    gpio_export(signal_pin, false);          


    irq_number = gpio_to_irq(signal_pin);     
    printk(KERN_INFO "%d pin mapped to IRQ: %d\n",signal_pin, irq_number);

    result = request_irq(irq_number, (irq_handler_t) signal_pin_irq_handler, IRQF_TRIGGER_RISING,  "my_interrupt",  NULL); 
    return result;
}

static void __exit cadriver_exit(void) 
{
    printk(KERN_INFO "Coin acceptor module is unloaded\n");
}

static irq_handler_t signal_pin_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    printk(KERN_INFO "Coin accepted\n");
    ca_counter++;                    
    return (irq_handler_t) IRQ_HANDLED; 
}


module_init(cadriver_init);
module_exit(cadriver_exit);
