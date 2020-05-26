#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "ioctl_ops.h"
#include "irq_hndlr.h"

#define IOBASE   0xfe000000
#define GPIO     0x200000
#define PIN      14

static unsigned int set_offset = 0x1C;
static unsigned int clr_offset = 0x28;

static void* base = (void*)(IOBASE + GPIO);
static volatile unsigned int* gpfsel;
static volatile unsigned int* gpset;
static volatile unsigned int* gpclr;
static unsigned int bit = PIN % 32;


static void setup_gpio(char output)
{
    unsigned int bit = (PIN % 10)*3;
    if(output) *gpfsel |= 1 << bit;
    else *gpfsel &= ~(1 << bit);
}

static void write_gpio(int level)
{
    if(level)(*gpset) = (1 << bit);
    else (*gpclr) = (1 << bit);
}


#define TEST _IOW('t','x',int*)
#define GET_RESULTS  _IOR('t','y',long*)
#define DEVICE_NAME   "iotest"


static dev_t first;         // Global variable for the first device number 
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

static int iotest_open(struct inode *inode, struct file *file);
static int iotest_release(struct inode *inode, struct file *file);
static ssize_t iotest_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t iotest_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long iotest_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = iotest_read,
        .write          = iotest_write,
        .open           = iotest_open,
        .unlocked_ioctl = iotest_ioctl,
        .release        = iotest_release,
};

int init_ioctl(void)
{
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
    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    gpfsel = ioremap((int)(base + (PIN / 10)*4), 4);
    gpset = ioremap((int)(base + set_offset + (PIN / 32)*4), 4);
    gpclr = ioremap((int)(base + clr_offset + (PIN / 32)*4), 4);

    setup_gpio(1);


    return 0;

}
void uninit_ioctl(void)
{
    setup_gpio(0);
    iounmap(gpclr);
    iounmap(gpset);
    iounmap(gpfsel);
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
}

static int iotest_open(struct inode *inode, struct file *file)
{
    return 0;
}
 
static int iotest_release(struct inode *inode, struct file *file)
{
    return 0;
}
 
static ssize_t iotest_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    return -EINVAL;
}
static ssize_t iotest_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    return -EINVAL;
}

unsigned long* results = NULL;
unsigned int results_len;

static unsigned long calc_time_diff(unsigned long a, unsigned long b)
{
    if(a < b) return b - a;
    return b+1000-a;
}
 
static long iotest_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    unsigned long time;
    int i, *param;
    char timeout = 0;
    param = (int*)arg;
    switch(cmd) {
        case TEST:
            kfree(results);
            results = kmalloc(*param*sizeof(unsigned long), GFP_KERNEL);
            results_len = *param;
            for(i = 0; i < *param; ++i) {
                if(get_last_irq_level()) {
                    time = usecs();
                    write_gpio(0);
                    while(get_last_irq_level()) {
                        if(calc_time_diff(time, usecs()) > 500000) {
                            results[i] = 0;
                            timeout = 1;
                            break;
                        }
                    }
                    if(timeout) {
                        results[i] = 1000000;
                        timeout = 0;
                    } 
                    else results[i] = calc_time_diff(time, get_last_irq_time());
                }
                else {
                    time = usecs();
                    write_gpio(1);
                    while(!get_last_irq_level()) {
                        if(calc_time_diff(time, usecs()) > 500000) {
                            results[i] = 0;
                            timeout = 1;
                            break;
                        }
                    }
                    if(timeout) {
                        results[i] = 1000000;
                        timeout = 0;
                    } 
                    else results[i] = calc_time_diff(time, get_last_irq_time());
                }
            }
            break;
        case GET_RESULTS:
            if(copy_to_user(param, results, results_len*sizeof(unsigned long))) {
                printk(KERN_INFO "Copying was not successful!\n");
            }
            break;
    }
    return 0;
}