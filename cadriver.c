#include <linux/init.h>      
#include <linux/module.h>    
#include <linux/kernel.h>    

#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>


#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>


#define COOKIE_BUF_SIZE PAGE_SIZE

MODULE_LICENSE("GPL");      
MODULE_AUTHOR("Vladislav Turgenev");  
MODULE_DESCRIPTION("Pulse coin acceptor driver"); 
MODULE_VERSION("0.1");   







dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;


enum regime {
    TOTAL,
    COIN1,
    COIN2,
    COIN3,
    COIN4
};

enum regime current_regime;

#define COOKIE_BUF_SIZE PAGE_SIZE

ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);

struct file_operations fops = {
    .owner = THIS_MODULE,
	.read = fortune_read,
	.write = fortune_write,
};


char *cookie_buf;


int regime = 0;











ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
    printk(KERN_INFO "Reading from device\n");
    int len = sprintf(cookie_buf, "%d\n", regime);
    return simple_read_from_buffer(buf, count, f_pos, cookie_buf, len);
}

char * mystrtok(char * str, const char * delim)
{
    static char * last = 0;
    if (str) last = str;
    if ((last == 0) || (*last == 0)) 
        return 0;
    char * c = last;
    while(strchr(delim,*c)) 
        ++c;
    if (*c == 0) 
        return 0;
    char * start = c;
    while(*c && (strchr(delim,*c)==0)) 
        ++c;
    if (*c == 0)
    {
        last = c;
        return start;
    }
    *c = 0;
    last = c+1;
    return start;
}


int parse_set(char* str, unsigned long int* num, unsigned long int* val, unsigned long int* pulses)
{
    char sep [2]=" ";
    char *istr;

    istr = mystrtok(cookie_buf,sep); //set
 
    istr = mystrtok (NULL,sep); // n
    *num = simple_strtoul(istr, NULL, 10);
  
    istr = mystrtok (NULL,sep); // v
    *val = simple_strtoul(istr, NULL, 10);

    istr = mystrtok (NULL,sep); // p
    *pulses = simple_strtoul(istr, NULL, 10);

    return 0;
}

ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
    printk(KERN_INFO "Writing into chdevice\n");
	if (copy_from_user(&cookie_buf[0], buf, count))
	{
		return -EFAULT;
	}

	cookie_buf[count-1] = 0;

    printk(KERN_INFO "%s\n", cookie_buf);

    if (strstr(cookie_buf, "showtotal"))
        current_regime = TOTAL;
    else if (strstr(cookie_buf, "show1"))
        current_regime = COIN1;
    else if (strstr(cookie_buf, "show2"))
        current_regime = COIN2;
    else if (strstr(cookie_buf, "show3"))
        current_regime = COIN3;
    else if (strstr(cookie_buf, "show4"))
        current_regime = COIN4;
    else if (strstr(cookie_buf, "clear"))
    {}
    else if (strstr(cookie_buf, "time"))
    {}
    else if (strstr(cookie_buf, "set"))
    {
        printk(KERN_INFO "set parsing\n");   
        unsigned long int a,b,c;
        if (parse_set(cookie_buf, &a, &b, &c) == 0)
        {
		    printk(KERN_INFO "Success %lu %lu %lu\n", a,b,c);
        }
        else
		    printk(KERN_INFO "parse error\n");
    }
    else
        printk(KERN_INFO "unnknowh command\n");        

	return count;
}







struct coin_t
{
	int value;
	int pulse;
};

struct bank_t
{
	struct coin_t coin[4];
	int number[4];
};


static struct bank_t bank;

struct counter
{
    struct timer_list my_timer;
    int counting_time;   
    volatile int is_counting; 
    volatile int pulse_count;
};

void init_bank(struct bank_t *b)
{
	int i;
	for (i=0; i<4; i++)
		b->number[i] = 0;
		
	b->coin[0].value = 1;
	b->coin[0].pulse = 1;
	
	b->coin[1].value = 2;
	b->coin[1].pulse = 2;
	
	b->coin[2].value = 5;
	b->coin[2].pulse = 5;
	
	b->coin[3].value = 10;
	b->coin[3].pulse = 10;				
}

void timer_callback(unsigned long data);

void init_counter(struct counter *c)
{
    c->counting_time = 1200;
    c->is_counting = 0;
    c->pulse_count = 0;
    setup_timer(&(c->my_timer), timer_callback, 0 );
}

static struct timer_list my_timer;
static int counting_time = 1200;
static volatile int is_counting=0;
static volatile int pulse_count;


void timer_callback(unsigned long data)
{
	int i;
	for (i=0; i<4; i++)
	{
		printk(KERN_INFO "%d ", bank.coin[i].pulse);
		if (bank.coin[i].pulse == pulse_count)
			bank.number[i]++;
	}
			
    printk(KERN_INFO "got (%d) pulses.\n", pulse_count);
    
    printk(KERN_INFO "got %d - 1.\n %d - 2\n %d -5\n %d -10\n", bank.number[0], bank.number[1],bank.number[2],bank.number[3]);
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

    init_bank(&bank);

	cookie_buf = vmalloc(COOKIE_BUF_SIZE);
	if (!cookie_buf)
	{
		printk(KERN_INFO "Not enough memory for the cookie pot.\n");
		return -ENOMEM;
	}
	memset(cookie_buf, 0, COOKIE_BUF_SIZE);

    

    printk(KERN_INFO "Coin acceptor module is loaded\n");

    setup_timer( &my_timer, timer_callback, 0 );
    is_counting = 0;

    gpio_request(signal_pin, "sysfs");       
    gpio_direction_input(signal_pin);        
    gpio_export(signal_pin, false);          


    irq_number = gpio_to_irq(signal_pin);     
    printk(KERN_INFO "%d pin mapped to IRQ: %d\n",signal_pin, irq_number);

    result = request_irq(irq_number, (irq_handler_t) signal_pin_irq_handler, IRQF_TRIGGER_RISING,  "my_interrupt",  NULL); 



		/*Allocating Major number*/
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0)
	{
			printk(KERN_INFO "Cannot allocate major number for device\n");
			return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	cdev_init(&etx_cdev, &fops);

	if((cdev_add(&etx_cdev,dev,1)) < 0){
		printk(KERN_INFO "Cannot add the device to the system\n");
		unregister_chrdev_region(dev,1);
		result = -1;
	}

	/*Creating struct class*/
	if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL)
	{
		printk(KERN_INFO "Cannot create the struct class for device\n");
		unregister_chrdev_region(dev,1);
		result = -1;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL)
	{
		printk(KERN_INFO "Cannot create the Device\n");
		class_destroy(dev_class);
		unregister_chrdev_region(dev,1);
		result = -1;
	}

	printk(KERN_INFO "Kernel Module Inserted Successfully...\n");


    
    return result;
}

static void __exit cadriver_exit(void) 
{
    printk(KERN_INFO "Coin acceptor module is unloaded\n");
	
	free_irq(irq_number, NULL);
	gpio_unexport(signal_pin);
	gpio_free(signal_pin);


    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);

	if (cookie_buf)
	{
		vfree(cookie_buf);
	}
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
