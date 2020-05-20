#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

char flag = 1;

void intHandler(int _i) {
    flag = 0;
}

int main()
{
    signal(SIGINT, intHandler);

    //TODO init
    while(flag)
    {
        //TODO loop
    }
    //TODO cleanup
}