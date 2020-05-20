#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Levente Bajczi");
MODULE_DESCRIPTION("AMP loader module");
MODULE_VERSION("0.01");

#define DEVICE_NAME         "loader"
#define CORE3_MSGBOX_SET    0x400000bc
#define CORE3_MSGBOX_CLR    0x400000fc
#define BASE_ADDR           0x20000000

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static dev_t first;         // Global variable for the first device number 
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

static int device_open_count = 0;
static volatile unsigned int *msgbox_set;
static volatile unsigned int *msgbox_clr;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
   printk(KERN_ALERT "This device is write-only.\n");
   return -EINVAL;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    volatile unsigned int* data;
    int ok = 1, len_remaining;
    if(*msgbox_clr) 
    {
        printk(KERN_ALERT "You cannot interrupt a running program!\n");
        return -EBUSY;
    }
    data = ioremap(BASE_ADDR,len);
    if((len_remaining = copy_from_user((void*)data, buffer, len))){
        ok = -1;
        printk(KERN_ALERT "%d bytes still remain!\n", len_remaining);
    }
    iounmap(data);
    *msgbox_set = BASE_ADDR;
    printk(KERN_INFO "Baremetal program started!\n");
    return ok*len;
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init loader_init(void) {
    msgbox_set = ioremap(CORE3_MSGBOX_SET, 4);
    msgbox_clr = ioremap(CORE3_MSGBOX_CLR, 4);

    if (alloc_chrdev_region(&first, 0, 1, DEVICE_NAME) < 0)
    {
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, DEVICE_NAME)) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    if (device_create(cl, NULL, first, NULL, DEVICE_NAME) == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    cdev_init(&c_dev, &file_ops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    return 0;

}


static void __exit loader_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    iounmap(msgbox_set);
    iounmap(msgbox_clr);
}


module_init(loader_init);
module_exit(loader_exit);