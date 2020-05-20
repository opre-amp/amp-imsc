#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Levente Bajczi");
MODULE_DESCRIPTION("Mirrors GPIO 23 to 23");
MODULE_VERSION("0.01");

volatile char flag = 1;
struct task_struct* mirror_task;



static int mirror(void* param)
{
    //TODO init
    while(flag)
    {
        //TODO loop
    }
    //TODO cleanup
    return 0;
}



static int __init kernel_gpio_init(void)
{
    mirror_task = kthread_run(mirror, NULL, "Mirroring GPIO 15 to 23.");
    return 0;
}

static void __exit kernel_gpio_exit(void)
{
    flag = 0;
    kthread_stop(mirror_task);
}

module_init(kernel_gpio_init);
module_exit(kernel_gpio_exit);