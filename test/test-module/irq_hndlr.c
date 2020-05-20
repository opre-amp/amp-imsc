#include "irq_hndlr.h"
//#include <mach/platform.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/time.h>

#define IOBASE   0x3f000000
#define GPIO     0x200000

static int gpio;
static int irq_gpio;
static char irq_mapping_ok = 0;
static volatile unsigned int* gplvl;


static void* base = (void*)(IOBASE + GPIO);
static unsigned int lvl_offset = 0x34;

static unsigned int lastIRQtime = 0;
static int lastIRQlevel = 0;

static int read_gpio(void)
{
    unsigned int bit = gpio % 32;
    return (*gplvl) & (1 << bit);
}

unsigned long usecs(void) {
    struct timeval tv ;
    do_gettimeofday(&tv);
    return tv.tv_usec;
}

static irqreturn_t r_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
    unsigned long flags;
    local_irq_save(flags);

    lastIRQtime = usecs();
    lastIRQlevel = read_gpio();


    local_irq_restore(flags);
    return IRQ_HANDLED;
}

unsigned int get_last_irq_time(void) {
    return lastIRQtime;
}
int get_last_irq_level(void) {
    return lastIRQlevel;
}

void config_irq(int id, const char* descr)
{
    if(irq_mapping_ok) {
        printk(KERN_INFO "An IRQ handler is already present.\n");
        return;
    }
    if (gpio_request(id, descr)) {
        printk(KERN_INFO "GPIO request failure: %s\n", descr);
        return;
    }

    if ( (irq_gpio = gpio_to_irq(id)) < 0 ) {
        printk(KERN_INFO "GPIO to IRQ mapping failure %s\n", descr);
        irq_mapping_ok = 0;
        return;
    }
    gpio = id;
    irq_mapping_ok = 1;
    gplvl = ioremap((int)(base + lvl_offset + (gpio / 32)*4), 4);
    printk(KERN_INFO "Mapped int %d\n", irq_gpio);

    if (request_irq(irq_gpio,
                    (irq_handler_t ) r_irq_handler,
                    IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
                    descr,
                    NULL)) {
        printk(KERN_INFO "Irq Request failure\n");
        return;
    }
}
void release_irq(void)
{
   free_irq(irq_gpio, NULL);
   if (irq_mapping_ok) {
       gpio_free(gpio);
       iounmap(gplvl);
   } 
}