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

#include "button.h"
#include "fnd.h"
#include "colorled.h"
#include "led.h"
#include "textlcd.h"
#include "libBitmap.h"
#include "bitmapFileHeader.h"
#include "gyro.h"

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
static int mugunghwa_end = 0;   // 무궁화 꽃이 피었습니다 사운드 종료
static int stage3_end = 0;
static int step = 0;
static int running = 100;               // 참가자가 버튼 한번 누를때마다 달리는 거리에 쓰이는 변수
static char *data;
static int cols = 0, rows = 0;

static int new_x = 520;
static int new_y = 380;
static int accel_x= 0;
static int accel_y = 0;
static int dalgona_start = 0;

void *thread_object_2(){                //  버튼 입력 스레드
    while(stage3_end != 1){
		msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);               // 버튼 입력 메세지 받음
		if(rcv.keyInput == 3){                                                      // "Search" 버튼 눌리면
            if(mugunghwa_end == 1){                                         // 무궁화 음성이 끝나서 영희가 쳐다보고 있는데 버튼 눌리면(움직이면) 게임 실패 => 탈락
                lcdtextWrite("Stage 3", "FAILED");
		        printf("Stage Failed !! \n");
                system("sudo amixer sset 'Speaker' 40%");
                system("sudo aplay ./gunsound.wav");                 // Text Lcd에 "Failed" 출력하고 총맞는 음성 재생
                read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(2);
                read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                stage3_end = 1;
                break;
            }
           else if(mugunghwa_end == 0){                                 // 무조건 무궁화 음성이 나오는 중에 움직여야함
                 if(btn == 160){                                                        // 버튼 80번 누르면 성공 !!
                    printf("Stage 1 Clear !! \n\n");                        // Text Lcd에 "CLEARED" 출력
                    lcdtextWrite("Stage 3", "CLEARED");
                    stage3_end = 1;
                    break;
                }
                btn++;                                                                  // 3번 버튼 누르면 버튼 카운트 변수 +1
                running = running + 5;                                          // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용하는데, 버튼 한번 누를때 마다 running 변수 + 5
                read_bmp("456.bmp",&data,&cols,&rows);          // 참가자가 버튼 눌러 이동할때마다 새로 업데이트된 좌표로 참가자.bmp 출력 (TFT LCD)
                fb_write_b(data,cols,rows,running);
                
                if(btn %2 == 1){                                                       
                    printf("Key pressed for %d times\n", btn/2+1);
                }
            }
        }
	}
}

void *thread_object_1(){
    while(timer_end != 1){
        // y 좌표 테스트
        read_bmp("aim.bmp",&data,&cols,&rows);          // 참가자가 버튼 눌러 이동할때마다 새로 업데이트된 좌표로 참가자.bmp 출력 (TFT LCD)
        fb_write_c(data,cols,rows, new_x , new_y);
        accel_x = get_accel_x();
        accel_y = get_accel_y();

        printf("\n");
    
        
        if(accel_x >= 4000 ){
            printf("a : %d\n" ,accel_x);
            new_y  = new_y + 10;
        }
        else if(accel_x <= -4000){
            printf("a : %d\n" ,accel_x);
            new_y = new_y - 10;
        }
        else if(accel_y >= 4000){
            printf("b : %d\n" ,accel_y);
            new_x = new_x + 10;
        }
        else if(accel_y <= -4000){
            printf("b : %d\n" ,accel_y);
            new_x = new_x - 10;
        }

        read_bmp("aim.bmp",&data,&cols,&rows);       
        fb_write_c(data,cols,rows,new_x, new_y);
        printf("x: %d       y: %d\n\n",new_y, new_x);

        
        if(new_y >= 120 && new_y <= 380 && new_x == 650){
            printf("ok\n");
        }
        else if(new_y >= 120 && new_y <= 380 && new_x ==  390){
            printf("ok\n");
        }
        else if(new_y == 120 && new_x >= 390 && new_x <= 650){
            printf("ok\n");
        }
        else if(new_y == 380 && new_x >= 390 && new_x <= 650){
            printf("ok\n");
        }
        else if(new_y == 120 && new_x == 520){
            dalgona_start ++;            
        }
        else if(new_y == 380 && new_x ==  520 && dalgona_start != 0){
            printf("STAGE CLEAR\n\n");
            timer_end = 1;
            success = 1;
        }
        else{
            printf("die !!! \n\n");
            system("sudo amixer sset 'Speaker' 40%");
            system("sudo aplay ./gunsound.wav");     
            read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
            fb_write(data, cols,rows);
            sleep(2);
            read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
            fb_write(data, cols,rows);
            break;
            }
        usleep(125000);
    }
}

void *thread_object_0(){
    while(timer_end != 1){
        for(t=25; t>=0; t--){
        mode=MODE_STATIC_DIS;
        fndDisp(t,mode);
        sleep (1);
        }
        timer_end=1;
    }

    if(timer_end == 1 && success == 0){
            printf("die !!! \n\n");
            system("sudo amixer sset 'Speaker' 40%");
            system("sudo aplay ./gunsound.wav");     
            read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
            fb_write(data, cols,rows);
            sleep(2);
            read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
            fb_write(data, cols,rows);
    }
}

int main(void){
    btn = 0;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
  
    pthread_t thread[2];                        // 버튼 활용 위해 스레드
	int msdID = msgget (MESSAGE_ID, IPC_CREAT|0666);        // 버튼 입력 위해 메세지큐 생성

	buttonInit();
    pwmLedInit();
    ledLibInit();
    lcdtextInit();

    if ( fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0 )
   {
      printf ("FrameBuffer Init Failed\r\n");                   // TFT LCD 초기화
   }

    lcdtextWrite("GAME START", "");

    //read_bmp("squidgamestart.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면 :: 초대장 사진 출력
    //fb_write(data, cols,rows);

    read_bmp("dalgona.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면 :: 초대장 사진 출력
    fb_write(data, cols,rows);

    
    

    msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);                   // 버튼 입력`1

    while(1){
        int returnValue = 0;
        returnValue = msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT);           // 메세지큐 초기화
        if(returnValue == -1) break;
    }

    pthread_create(&thread[0],NULL,thread_object_0,NULL);                   // 버튼 입력 위한 스레드 생성
    pthread_create(&thread[1],NULL,thread_object_1,NULL);                   // 버튼 입력 위한 스레드 생성
    //pthread_create(&thread[2],NULL,thread_object_2,NULL);                   // 버튼 입력 위한 스레드 생성
    
    /*while(stage3_end != 1){         
        if(step == 3) {                         // 무궁화 꽃이 피었습니다 (5초 음성) 3번 반복했을때
            if(btn < 200){                      // 선까지 도달하지 못했으면 (실패)
                lcdtextWrite("Stage 3", "FAILED");      
		        printf("Stage Failed !! \n");
                system("sudo amixer sset 'Speaker' 40%");       
                system("sudo aplay ./gunsound.wav");                    // 총 맞는 소리 재생
                read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(2);
                read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
               stage3_end = 1;                                                 // 무궁화 꽃이 피었습니다 게임 종료
              break;
            }
        }
        step ++;                                    // 무궁화 꽃이 피었습니다 음성 출력 횟수 카운트
        mugunghwa_end = 0;             // 무궁화 꽃이 피었습니다 음성 출력 시작 플래그
        read_bmp("YH1.bmp", &data, &cols, &rows);       // 영희가 벽 보고 있는 사진.bmp 출력 (TFT LCD)
        fb_write(data, cols,rows);
        read_bmp("456.bmp",&data,&cols,&rows);          // 참가자.bmp 출력 (TFT LCD)
        fb_write_b(data,cols,rows,running);                     // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용 
        system("sudo amixer sset 'Speaker' 40%");           
        system("sudo aplay ./redlightgreenlight.wav");      // 무궁화 꽃이 피었습니다 음성 출력
        mugunghwa_end = 1;                                              // 무궁화 꽃이 피었습니다 음성 출력 종료 플래그
        read_bmp("YH2.bmp", &data, &cols, &rows);       // 영희가 뒤돌아보고 있는 사진.bmp 출력 (TFT LCD)
        fb_write(data, cols,rows);
        read_bmp("456.bmp",&data,&cols,&rows);           // 참가자.bmp 출력 (TFT LCD)
        fb_write_b(data,cols,rows,running);                      // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용 
        sleep(3);                                                                   // 다음 무궁화 꽃이 피었습니다 음성 출력 전 3초 대기
    }
*/

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    //pthread_join(thread[2], NULL);
    pwmInactiveAll();
    buttonExit();
    ledLibExit();
    lcdtextExit();
    close_bmp();
    fb_close();   

    return 0;
}
