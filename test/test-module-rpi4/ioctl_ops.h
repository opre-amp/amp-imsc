#ifndef IOCTL_OPS_H
#define IOCTL_OPS_H

#include <linux/io.h>

/*
 * Before and after using the ioctl ops, call these functions
 */
int init_ioctl(void);
void uninit_ioctl(void);

#endif