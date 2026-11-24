
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "raise_char"
#define CLASS_NAME  "raise_char_class"
#define BUFFER_SIZE 256

static dev_t dev_num;
static struct cdev raise_cdev;
static struct class *raise_class;
static struct device *raise_device;

static char device_buffer[BUFFER_SIZE] = "Hello from RAISE LAB!";
static size_t buffer_length = 21;

// OPEN
static int raise_open(struct inode *inode, struct file *file)
{
    pr_info("RAISE LAB: Device opened\n");
    return 0;
}

// READ
static ssize_t raise_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{
    size_t bytes;

    if (*offset >= buffer_length)
        return 0;

    bytes = min(count, buffer_length - (size_t)*offset);

    if (copy_to_user(user_buffer, device_buffer + *offset, bytes))
        return -EFAULT;

    *offset += bytes;

    pr_info("RAISE LAB: Data read\n");

    return bytes;
}

// WRITE
static ssize_t raise_write(struct file *file,
                           const char __user *user_buffer,
                           size_t count,
                           loff_t *offset)
{
    size_t bytes = min(count, (size_t)BUFFER_SIZE - 1);

    if (copy_from_user(device_buffer, user_buffer, bytes))
        return -EFAULT;

    device_buffer[bytes] = '\0';
    buffer_length = bytes;

    pr_info("RAISE LAB: Data written\n");

    return bytes;
}

// RELEASE
static int raise_release(struct inode *inode, struct file *file)
{
    pr_info("RAISE LAB: Device closed\n");
    return 0;
}

// FILE OPERATIONS
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = raise_open,
    .read    = raise_read,
    .write   = raise_write,
    .release = raise_release,
};

// DRIVER INITIALIZATION
static int __init raise_driver_init(void)
{
    int ret;

    pr_info("RAISE LAB: Initializing Character Driver\n");

    // Allocate major and minor numbers
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    if (ret < 0)
        return ret;

    pr_info("RAISE LAB: Major = %d, Minor = %d\n",
            MAJOR(dev_num), MINOR(dev_num));

    // Initialize character device
    cdev_init(&raise_cdev, &fops);
    raise_cdev.owner = THIS_MODULE;

    ret = cdev_add(&raise_cdev, dev_num, 1);

    if (ret < 0)
        goto error_region;

    // Create device class (Linux 6.4+ API)
    raise_class = class_create(CLASS_NAME);

    if (IS_ERR(raise_class)) {
        ret = PTR_ERR(raise_class);
        goto error_cdev;
    }

    // Create device node
    raise_device = device_create(
        raise_class,
        NULL,
        dev_num,
        NULL,
        DEVICE_NAME
    );

    if (IS_ERR(raise_device)) {
        ret = PTR_ERR(raise_device);
        goto error_class;
    }

    pr_info("RAISE LAB: Character Driver Loaded\n");

    return 0;

error_class:
    class_destroy(raise_class);

error_cdev:
    cdev_del(&raise_cdev);

error_region:
    unregister_chrdev_region(dev_num, 1);

    return ret;
}

// DRIVER EXIT
static void __exit raise_driver_exit(void)
{
    device_destroy(raise_class, dev_num);
    class_destroy(raise_class);
    cdev_del(&raise_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("RAISE LAB: Character Driver Removed\n");
}

module_init(raise_driver_init);
module_exit(raise_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAISE LAB");
MODULE_DESCRIPTION("RAISE LAB Linux Character Device Driver");
MODULE_VERSION("1.0");
