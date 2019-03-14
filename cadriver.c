#include <linux/init.h>      
#include <linux/module.h>    
#include <linux/kernel.h>    

#include <linux/interrupt.h>
#include <linux/gpio.h>

#include <linux/timer.h>

MODULE_LICENSE("GPL");      
MODULE_AUTHOR("Vladislav Turgenev");  
MODULE_DESCRIPTION("Pulse coin acceptor driver"); 
MODULE_VERSION("0.1");     

static struct timer_list my_timer;
static int counting_time = 1200;
static volatile int is_counting=0;
static volatile int pulse_count;


void timer_callback(unsigned long data)
{
    printk(KERN_INFO "got (%d) pulses.\n", pulse_count);
    is_counting = 0;
}

static int pin_numbers[] = {18, 27};

static volatile int ca_counter = 0;

static int irq_number; 
static unsigned int signal_pin = 18;    // pin 12 

static irq_handler_t signal_pin_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int __init cadriver_init(void) 
{
    int result = 0;

    printk(KERN_INFO "Coin acceptor module is loaded\n");

    setup_timer( &my_timer, timer_callback, 0 );
    is_counting = 0;

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
	
	free_irq(irq_number, NULL);
	gpio_unexport(signal_pin);
	gpio_free(signal_pin);
}

static irq_handler_t signal_pin_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    ca_counter++;
    if (is_counting == 0)
    {
        is_counting = 1;
        pulse_count = 0;
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(counting_time)); 
    }
    pulse_count++; 

    printk(KERN_INFO "Pulse accepted %d %d %d\n",ca_counter, pulse_count, is_counting);             
    return (irq_handler_t) IRQ_HANDLED; 
}


module_init(cadriver_init);
module_exit(cadriver_exit);
