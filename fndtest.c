#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <linux/input.h>
#include <sys/msg.h>
#include <pthread.h>
#include <fcntl.h>
#include "fnd.h"
#define FND_DRIVER_NAME		"/dev/perifnd"

#define MODE_STATIC_DIS		0

int main(){
    while(1){
        fndDisp(221130, 0b101010);
    }
    return 0;
}