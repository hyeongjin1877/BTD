#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/types.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>

#include "touch.h"
#include "button.h"
#include "fnd.h"
#include "colorled.h"
#include "led.h"
#include "textlcd.h"
#include "libBitmap.h"
#include "bitmapFileHeader.h"

#define MODE_STATIC_DIS		0
static int msgID;
BUTTON_MSG_T rcv;
static int timer_end=0;
static int mode;
static int t;
static int result=0;
static int success= 0;
static int btn = 0;
static int theend = 0;
static int mugunghwa_end = 0;
static int stage3_end = 0;
static int step = 0;
static int game_start = 0;

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

void *thread_object_2(){
    while(stage3_end != 1){
		msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);
		if(rcv.keyInput == 3){
            if(mugunghwa_end == 1){
                lcdtextWrite("Stage 3", "FAILED");
		        printf("Stage Failed !! \n");
                system("sudo amixer sset 'Speaker' 40%");
                system("sudo aplay ./gunsound.wav");
                stage3_end = 1;
                break;
            }
           else if(mugunghwa_end == 0){
                 if(btn == 200){
                    printf("Stage 1 Clear !! \n\n");
                    lcdtextWrite("Stage 3", "CLEARED");
                    stage3_end = 1;
                    break;
                }
                btn++;
                if(btn %2 == 1){
                    printf("Key pressed for %d times\n", btn/2+1);
                }
            }
        }
	}
}

int main(void){
    btn = 0;
    int cols = 0, rows = 0;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    char *data;
    pthread_t thread[2];
    BUTTON_MSG_TOUCH rcv_TOUCH;
    int msgID_TOUCH = msgget(MESSAGE_ID_TOUCH, IPC_CREAT|0666);
	int msgID = msgget (MESSAGE_ID, IPC_CREAT|0666);

	buttonInit();
    pwmLedInit();
    ledLibInit();
    lcdtextInit();
    touchInit();

    if ( fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0 )
   {
      printf ("FrameBuffer Init Failed\r\n");
   }

    while(1){
        int returnValue = 0;
        returnValue = msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT);
        if(returnValue == -1) break;
    }

    while(1){
        int returnValue_t = 0;
        returnValue_t = msgrcv(msgID_TOUCH, &rcv_TOUCH, sizeof(rcv_TOUCH)-sizeof(long int), 0, 0);
        if(returnValue_t == -1) break;
    }

    lcdtextWrite("GAME START", "");
    read_bmp("squidgamestart.bmp", &data, &cols, &rows);
    fb_write(data, cols,rows);

    while(game_start != 1){
        switch(rcv_TOUCH.keyInput){
        case 999:
            if( rcv_TOUCH.pressed == 1)
            {
                if(rcv_TOUCH.x > 705) printf("NOPE\r\n");
                else if (rcv_TOUCH.y > 400) printf("NOPE\r\n");
                else if (rcv_TOUCH.x < 320) printf("NOPE\r\n");
                else {
                    printf("START!\r\n");
                    game_start = 1;
                }
            }
    }    
    }

    msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);

	//pthread_create(&thread[0],NULL,thread_object_0,NULL);
    //pthread_create(&thread[1],NULL,thread_object_1,NULL);
    pthread_create(&thread[2],NULL,thread_object_2,NULL);

    printf("\n\naudio start\n\n");
    
    while(stage3_end != 1){
        if(step == 3) {
            if(btn < 200){
                lcdtextWrite("Stage 3", "FAILED");
		        printf("Stage Failed !! \n");
                system("sudo amixer sset 'Speaker' 40%");
                system("sudo aplay ./gunsound.wav");
               stage3_end = 1;
              break;
            }
        }
        step ++;
        mugunghwa_end = 0;
        read_bmp("YH1.bmp", &data, &cols, &rows);
        fb_write(data, cols,rows);
        system("sudo amixer sset 'Speaker' 40%");
        system("sudo aplay ./redlightgreenlight.wav");
        mugunghwa_end = 1;
        read_bmp("YH2.bmp", &data, &cols, &rows);
        fb_write(data, cols,rows);
        sleep(3);
    }

    
    //system("sudo amixer sset 'Speaker' 20%");
    //usleep(1850000);
    //system("sudo aplay ./bgm.wav"); 

    //pthread_join(thread[0], NULL);
    //pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pwmInactiveAll();
    buttonExit();
    ledLibExit();
    lcdtextExit();
    touchExit();
    close_bmp();
    fb_close();    
    return 0;
}
