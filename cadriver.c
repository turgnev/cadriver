#include <linux/init.h>      
#include <linux/module.h>    
#include <linux/kernel.h>    

MODULE_LICENSE("GPL");      
MODULE_AUTHOR("Vladislav Turgenev");  
MODULE_DESCRIPTION("Pulse coin acceptor driver"); 
MODULE_VERSION("0.1");       

static int __init cadriver_init(void) {
   printk(KERN_INFO "Coin acceptor module is loaded\n");
   return 0;
}

static void __exit cadriver_exit(void) {
   printk(KERN_INFO "Coin acceptor module is unloaded\n");
}

module_init(cadriver_init);
module_exit(cadriver_exit);
