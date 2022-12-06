#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>

#include "button.h"
#include "fnd.h"
#include "textlcddrv.h"
#include "colorled.h"
#include "led.h"

#define FND_DRIVER_NAME		"/dev/perifnd"
#define LED_DRIVER_NAME "/dev/periled"
#define MODE_STATIC_DIS		0
static int msgID;

volatile int timer_end=0;
int mode;
int t;
int result=0;
int success= 0;

void *thread_object_0(){            // 스레드 사용해서 컬러 led 별개 동작
   pwmSetPercent(0, 0);
    pwmSetPercent(100, 1);
    pwmSetPercent(0, 2);
    for(int j=0; j<8; j++){
        ledOnOff(j,1);
    }
    sleep(5);
    pwmSetPercent(100, 0);
    pwmSetPercent(0, 1);
    pwmSetPercent(0, 2);    
    for(int j=0; j<8; j++){
        ledOnOff(j,0);
    }
    sleep(5);
    pwmSetPercent(0, 0);
    pwmSetPercent(0, 1);
    pwmSetPercent(100, 2);
    for(int j=0; j<4; j++){
        ledOnOff(j,1);
    }
    sleep(5);
}

void *thread_object_1(){            // 스레드 사용해서 fnd 에 30초 타이머 출력
   for(t=30; t>=0; t--){
      mode=MODE_STATIC_DIS;
      fndDisp(t,mode);
      sleep (1);
   }
   	timer_end=1;
   if(success == 0){
		printf("Stage Failed !! \n");
   }
}

int main(void){

    pthread_t thread[2];
	BUTTON_MSG_T rcv;
	int msdID = msgget (MESSAGE_ID, IPC_CREAT|0666);

	buttonInit();
    pwmLedInit();
    ledLibInit();
	int i = 0;
	printf("\nPRESS Search Button 50 times in 30 sec\n\n");

	pthread_create(&thread[0],NULL,thread_object_0,NULL);
    pthread_create(&thread[1],NULL,thread_object_1,NULL);
	
    while(1){	
        if(timer_end == 1){
				break;
		}
		msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);
		if(rcv.keyInput == 3){
			i++;
			if( i == 100){
				success = 1;
				printf("Stage Clear !! \n\n");
				break;
			}
			else if(i %2 == 1){
				if(timer_end == 1){
					break;
				}
				printf("Key pressed for %d times\n", i/2+1);
			}
		}
	}
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pwmInactiveAll();
    buttonExit();
    ledLibExit();
     
    return 0;
}
