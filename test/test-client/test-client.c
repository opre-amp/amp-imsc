#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <stdarg.h>
#include <stdlib.h>



#define TEST _IOW('t','x',int*)
#define GET_RESULTS  _IOR('t','y',long*)
#define DEVICE_NAME   "/dev/iotest"

static int fd;

int main(int argc, char** argv){
    if(argc == 1) {
        printf("Please specify number of test runs: ./test-client <N>\n");
        return -1;
    }
    int num = atoi(argv[1]);
    fd = open(DEVICE_NAME, O_RDWR);
    if(fd < 0) {
        return -1;
    }
    ioctl(fd, TEST, &num);
    unsigned long results[num];
    ioctl(fd, GET_RESULTS, results);
    for(int i = 0; i < num; ++i) printf("%d: %lu\n", i, results[i]);
    close(fd);
}