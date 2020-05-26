#include "irq_hndlr.h"
//#include <mach/platform.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/time.h>

#define IOBASE   0xfe000000
#define GPIO     0x200000

static volatile unsigned int* gplvl;

static int gpio;
static void* base = (void*)(IOBASE + GPIO);
static unsigned int lvl_offset = 0x34;

static unsigned int lastIRQtime = 0;
static int lastIRQlevel = 0;

volatile char flag = 1;
struct task_struct* reader_task;

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

static int reader(void* param) {
    int savedIRQlevel;
    savedIRQlevel = 0;
    while(flag) {
        lastIRQlevel = read_gpio();
        if(lastIRQlevel != savedIRQlevel)
            lastIRQtime = usecs();
        savedIRQlevel = lastIRQlevel;
    }
    return 0;
}

unsigned int get_last_irq_time(void) {
    return lastIRQtime;
}
int get_last_irq_level(void) {
    return lastIRQlevel;
}

void config_irq(int id, const char* descr)
{
    gpio = id;
    gplvl = ioremap((int)(base + lvl_offset + (id / 32)*4), 4);
    reader_task = kthread_run(reader, NULL, "Reading GPIO instead of IRQ handler");
}
void release_irq(void)
{
   flag = 0;
   kthread_stop(reader_task);  
   iounmap(gplvl);
}