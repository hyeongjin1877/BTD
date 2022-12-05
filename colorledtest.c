#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "colorled.h"

int main(void){
    /*if(argc != 4){
        printf("colorledtest.elf 0-100 0-100 0-100\r\n");
        printf("ex) colorledtest.elf 100 100 100 ==> full white color\r\n");
        return 0;
    }*/
    pwmLedInit();
    /*pwmSetPercent(atoi(argv[1]),0);
    pwmSetPercent(atoi(argv[2]),1);
    pwmSetPercent(atoi(argv[3]),2);*/
    pwmSetPercent(0, 0);
    pwmSetPercent(100, 1);
    pwmSetPercent(0, 2);
    sleep(5);
    pwmSetPercent(100, 0);
    pwmSetPercent(0, 1);
    pwmSetPercent(0, 2);    
    sleep(5);
    pwmSetPercent(0, 0);
    pwmSetPercent(0, 1);
    pwmSetPercent(100, 2);
    sleep(5);
    pwmInactiveAll();
    return 0;
}
