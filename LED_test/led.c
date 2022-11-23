#include "led.h"
#include <fcntl.h>

static unsigned int ledValue = 0;
static int fd = 0;

int ledOnOff (int ledNum, int onOff){
    int i=1;
    i = i<<ledNum;
    ledValue = ledValue& (~i);
    if (onOff !=0) ledValue |= i;
    write (fd, &ledValue, 4);
    return 1;
}

int ledLibInit(void){
    fd=open("/dev/periled", O_RDWR);
    ledValue = 0;
    return 1;
}

int ledStatus(void){
    return ledValue;
}

int ledLibExit(void){
    ledValue = 0;     
    ledOnOff (0, 0);
    close(fd);
}