
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

// Module initialization function
static int __init hello_init(void)
{
    printk(KERN_INFO "RAISE LAB: Hello Linux Kernel!\n");

    return 0;
}

// Module cleanup function
static void __exit hello_exit(void)
{
    printk(KERN_INFO "RAISE LAB: Kernel Module Removed!\n");
}

// Register initialization and cleanup functions
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAISE LAB");
MODULE_DESCRIPTION("My First Linux Kernel Driver");
MODULE_VERSION("1.0");
