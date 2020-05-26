#ifndef IRQ_HNDLR_H
#define IRQ_HNDLR_H


void config_irq(int id, const char* descr);
void release_irq(void);

unsigned int get_last_irq_time(void);
int get_last_irq_level(void);

unsigned long usecs(void);

#endif