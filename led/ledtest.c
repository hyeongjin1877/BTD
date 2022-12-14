#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define LED_DRIVER_NAME "/dev/periled"

int main(){
    ledLibInit();
    sleep(3);

    ledOnOff(1,1);
    sleep(1);
    ledOnOff(1,0);
    sleep(1);
    ledOnOff(1,1);
    sleep(1);
    ledOnOff(1,0);
    sleep(1);
    ledOnOff(2,1);
    sleep(1);
    ledOnOff(2,0);
    sleep(1);
    ledOnOff(5,1);
    sleep(1);
    ledOnOff(5,0);
    sleep(1);

    ledLibExit();
    return 0;
    }



