#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include "button.h"
#include "fnd.h"
#include <sys/msg.h>
#include <pthread.h>

#define FND_DRIVER_NAME		"/dev/perifnd"
#define MODE_STATIC_DIS		0
static int msgID;

volatile int timer_end=0;
int mode;
int t;
int result=0;
int success= 0;

void *thread_object_2(){      
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
	int i = 0;
	printf("\nPRESS Search Button 50 times in 30 sec\n\n");

	pthread_create(&thread[1],NULL,thread_object_2,NULL);

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

/*
	while(1){
		msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);
		switch(rcv.keyInput){
			case 1:
				printf("VOLUME UP\n");
				break;
			case 2:
				printf("HOME\n");
				break;
			case 3:
				printf("SEARCH\n");
				break;
			case 4:
				printf("BACK\n");
				break;
			case 5:
				printf("MENU\n");
				break;
			case 6:
				printf("VOLUME DOWN\n");
				break;
		}
		msgrcv(msgID, &rcv.pressed, sizeof(rcv.pressed), 0, 0);
		switch(rcv.pressed){
			case 1:
				printf("BUTTON PRESSED\n\n");
				break;
			case 2:
				printf("BUTTON UNPRESSED\n\n");
				break;
		}
	}
	*/
buttonExit();
}
