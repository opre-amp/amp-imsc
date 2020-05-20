#include <linux/module.h>
#include <linux/io.h>

#include "ioctl_ops.h"
#include "irq_hndlr.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Levente Bajczi");
MODULE_DESCRIPTION("Tests the mirroring latency");
MODULE_VERSION("0.01");


static int __init test_init(void)
{
    config_irq(24, "IRQ for pin 24");
    return init_ioctl();
}

static void __exit test_exit(void)
{
    uninit_ioctl();
    release_irq();
}

module_init(test_init);
module_exit(test_exit);