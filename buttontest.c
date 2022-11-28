#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include "button.h"
#include <sys/msg.h>
#include <pthread.h>

static int msgID;

int main(void){
	BUTTON_MSG_T rcv;
	int msdID = msgget (MESSAGE_ID, IPC_CREAT|0666);

	buttonInit();

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
buttonExit();
}
