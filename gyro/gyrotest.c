#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "gyro.h"

int main(void){
    while(1){
        gyro_options();
        printf("\n");
        sleep(1);
    }
    return 0;
}