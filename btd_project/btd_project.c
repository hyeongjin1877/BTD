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
#include "touch.h"

#define MODE_STATIC_DIS		0
static int msgID;
BUTTON_MSG_T rcv;
static int timer_end=0;
static int mode;
static int t;
static int fail = 0;
static int finish = 0;
static int btn = 0;
static int mugunghwa_end = 0;   // 무궁화 꽃이 피었습니다 사운드 종료
static int stage1_end = 0;
static int step = 0;
static int running = 100;               // 참가자가 버튼 한번 누를때마다 달리는 거리에 쓰이는 변수
static char *data;
static int cols = 0, rows = 0;
static int stage_2 = 0;                     //stage_2 = 1 이 되면 달고나 스테이지 시작 
static int loop_break;

static int new_x = 520;
static int new_y = 380;
static int accel_x= 0;
static int accel_y = 0;
static int dalgona_start = 0;

pthread_t thread[2];                        // fnd, 버튼 활용 위한 스레드

void *thread_object_0(){                 // 스레드 0 :: FND를 활용한 타이머 생성
    if(stage_2 == 1){
        while(timer_end != 1){
            for(t=25; t>=0; t--){               // 25초 타이머 생성
            mode=MODE_STATIC_DIS;
            fndDisp(t,mode);
            sleep (1);
            }
            timer_end=1;
            break;
        }

        if(timer_end == 1 && fail == 0 && finish == 0){         // 실패하지 않았지만 시간내에 성공하지 못했으면
                printf("die !!! \n\n");                                         // 그래도 게임 오버
                lcdtextWrite("Stage 2", "FAILED");
                pwmSetPercent(0, 0);                                // 탈락을때 컬러 LED :: Red
                pwmSetPercent(0, 1);
                pwmSetPercent(100, 2);
                system("sudo amixer sset 'Speaker' 40%");
                system("sudo aplay ./gunsound.wav");     
                read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(2);
                read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
        }
    }
}

void *thread_object_1(){                                //  스레드 1 :: 무궁화 꽃이 피었습니다를 위한 버튼 입력 스레드
    while(loop_break != 1){
		msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);          // 버튼 입력 메세지 받음
		if(rcv.keyInput == 3){                                                                   // "Search" 버튼 눌리면
            if(mugunghwa_end == 1){                                                     // 무궁화 음성이 끝나서 영희가 쳐다보고 있는데 버튼 눌리면(움직이면) 게임 실패 => 탈락
                lcdtextWrite("Stage 1", "FAILED");
		        printf("Stage Failed !! \n");
                pwmSetPercent(0, 0);                                // 탈락했을때 컬러 LED :: Red
                pwmSetPercent(0, 1);
                pwmSetPercent(100, 2);
                stage1_end = 1;
                system("sudo amixer sset 'Speaker' 40%");
                system("sudo aplay ./gunsound.wav");                 // Text Lcd에 "Failed" 출력하고 총맞는 음성 재생
                read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(2);
                read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                loop_break = 1;
                break;
            }
           else if(mugunghwa_end == 0){                                 // 무조건 무궁화 음성이 나오는 중에 움직여야함
                 if(btn == 20){                                                        // 버튼 80번 누르면 성공 !!
                    printf("Stage 1 Clear !! \n\n");                        // Text Lcd에 "CLEARED" 출력
                    lcdtextWrite("Stage 1", "CLEAR");
                    stage_2 = 1;                                                    // <무궁화 꽃이 피었습니다> 스테이지를 클리어 후, 다음 스테이지 시작을 위한 플래그
                    stage1_end = 1; 
                    loop_break = 1;
                }
                btn++;                                                                  // 3번 버튼 누르면 버튼 카운트 변수 +1
                running = running + 5;                                          // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용하는데, 버튼 한번 누를때 마다 running 변수 + 5
                read_bmp("456.bmp",&data,&cols,&rows);          // 참가자가 버튼 눌러 이동할때마다 새로 업데이트된 좌표로 참가자.bmp 출력 (TFT LCD)
                fb_write_b(data,cols,rows,running);
                
                if(btn %2 == 1){                                                       
                    printf("Key pressed for %d times\n", btn/2+1);
                }

                 if(btn >= 20 && btn < 39){                                      // 버튼 10번 눌릴때마다 led 순서대로 하나씩 점등
                    ledOnOff(0,1);                                                    // LED 8개 모두 점등되면 80번 클릭 되었다는 의미 => 통과 !
                } else if(btn >= 40 && btn < 59){
                    ledOnOff(1,1);
                } else if(btn >= 60 && btn < 79){
                    ledOnOff(2,1);
                } else if(btn >= 80 && btn < 99){
                    ledOnOff(3,1);
                } else if(btn >= 100 && btn < 119){
                    ledOnOff(4,1);
                } else if(btn >= 120 && btn < 139){
                    ledOnOff(5,1);
                } else if(btn >= 140 && btn < 159){
                    ledOnOff(6,1);
                } else if(btn >= 160 && btn < 179){
                    ledOnOff(7,1);
                }
            }
        }
	}
}

int main(void){
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
  
	int msdID = msgget (MESSAGE_ID, IPC_CREAT|0666);        // 버튼 입력 위해 메세지큐 생성
    int msgID_TOUCH = msgget(MESSAGE_ID_TOUCH,IPC_CREAT|0666);
	TOUCH_MSG_T recvMsg_TOUCH;

    buttonInit();
    pwmLedInit();
    ledLibInit();
    lcdtextInit();
    touchInit();
    ledalloff();


    if ( fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0 )
    {
      printf ("FrameBuffer Init Failed\r\n");                                           // TFT LCD 초기화
    }

    lcdtextWrite("GAME START", "");

    read_bmp("squidgamestart.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면 :: 초대장 사진 출력
    fb_write(data, cols,rows);

    static int start_touch = 0;

    while(start_touch != 1)
    {
        msgrcv(msgID_TOUCH, &recvMsg_TOUCH, sizeof(recvMsg_TOUCH)-sizeof(long int), 0, 0);

        switch(recvMsg_TOUCH.keyInput)
        {
            case 999:
            
                if( recvMsg_TOUCH.pressed == 1)
                {
                    if(recvMsg_TOUCH.x > 705) printf("NOPE\r\n");
                    else if (recvMsg_TOUCH.y > 400) printf("NOPE\r\n");
                    else if (recvMsg_TOUCH.x < 320) printf("NOPE\r\n");
                    //else if (recvMsg.y > 500) printf("NOPE\r\n");
                    else{ 
                        start_touch = 1;
                    printf("START!\r\n");
                    }
                }
        }     
    }

    while(1){
        int returnValue = 0;
        returnValue = msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT);           // 메세지큐 초기화
        if(returnValue == -1) break;
    }

    pwmSetPercent(0, 0);                            // 살아있을때 컬러 LED :: Green
    pwmSetPercent(100, 1);
    pwmSetPercent(0, 2);

    pthread_create(&thread[1],NULL,thread_object_1,NULL);                   // 버튼 입력 위한 스레드 생성
    
    lcdtextWrite("GAME START", "STAGE 1"); 
    ///////// < 무궁화 꽃이 피었습니다> /////////
    while(stage1_end != 1){         
        if(step == 3) {                         // 무궁화 꽃이 피었습니다 (5초 음성) 3번 반복했을때
            if(btn < 160){                      // 선까지 도달하지 못했으면 (실패)
                lcdtextWrite("Stage 1", "FAILED");      
                stage1_end = 1;    
                loop_break = 1;                                             // 무궁화 꽃이 피었습니다 게임 종료
		        printf("Stage Failed !! \n");
                pwmSetPercent(0, 0);                                // 탈락했을때 컬러 LED :: Red
                pwmSetPercent(0, 1);
                pwmSetPercent(100, 2);
                system("sudo amixer sset 'Speaker' 40%");       
                system("sudo aplay ./gunsound.wav");                    // 총 맞는 소리 재생
                read_bmp("end1.bmp", &data, &cols, &rows);      // end1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(1);
                read_bmp("end2.bmp", &data, &cols, &rows);      // end2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                break;
            }
        }
        mugunghwa_end = 0;                                              // 무궁화 꽃이 피었습니다 음성 출력 시작 플래그
        read_bmp("YH1.bmp", &data, &cols, &rows);       // 영희가 벽 보고 있는 사진.bmp 출력 (TFT LCD)
        fb_write(data, cols,rows);
        read_bmp("456.bmp",&data,&cols,&rows);           // 참가자.bmp 출력 (TFT LCD)
        fb_write_b(data,cols,rows,running);                       // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용 
        system("sudo amixer sset 'Speaker' 40%");           
        system("sudo aplay ./redlightgreenlight.wav");      // 무궁화 꽃이 피었습니다 음성 출력
        mugunghwa_end = 1;                                              // 무궁화 꽃이 피었습니다 음성 출력 종료 플래그
        read_bmp("YH2.bmp", &data, &cols, &rows);       // 영희가 뒤돌아보고 있는 사진.bmp 출력 (TFT LCD)
        fb_write(data, cols,rows);
        read_bmp("456.bmp",&data,&cols,&rows);           // 참가자.bmp 출력 (TFT LCD)
        fb_write_b(data,cols,rows,running);                      // 참가자 사진 x 좌표 이동하기 위해 "running" 인자 사용 
        step ++;                                                                      // 무궁화 꽃이 피었습니다 음성 출력 횟수 카운트
        sleep(3);                                                                   // 다음 무궁화 꽃이 피었습니다 음성 출력 전 3초 대기
    }

    ///////// < 달고나 게임 > /////////

    if(stage_2 == 1){                               // 달고나 게임 스테이지로 넘어왔다는 플래그가 1이 되면 실행
        if ( fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0 )
        {
            printf ("FrameBuffer Init Failed\r\n");                   // TFT LCD 초기화
        }
        lcdtextWrite("GAME START", "STAGE 2");          
        pthread_create(&thread[0],NULL,thread_object_0,NULL);            // fnd 스레드 생성
        read_bmp("dalgona.bmp", &data, &cols, &rows);                        // 달고나 스테이지 배경 :: 달고나 사진 출력
        fb_write(data, cols,rows);
                
        while(timer_end != 1){  
            read_bmp("aim.bmp",&data,&cols,&rows);         // 달고나 게임에서 사용되는 선 긋는 용 bmp 출력
            fb_write_c(data,cols,rows, new_x , new_y);
                
            accel_x = get_accel_x();                                    // 가속도 센서 값 불러옴
            accel_y = get_accel_y();
            printf("\n");
                
            if(accel_x >= 4000 ){                       // !! 사용자가 키트를 쥐고 TFT LCD를 바라보는 시점에서의
                printf("a : %d\n" ,accel_x);      // x축 = new_y ::: y축 = new_x
                new_y  = new_y + 10;                
            }
            else if(accel_x <= -4000){
                printf("a : %d\n" ,accel_x);
                new_y = new_y - 10;
            }
            else if(accel_y >= 4000){               // 사용자가 키트를 기울이는 방향대로 aim이 이동
                printf("b : %d\n" ,accel_y);
                new_x = new_x + 10;
            }
            else if(accel_y <= -4000){
                printf("b : %d\n" ,accel_y);
                new_x = new_x - 10;
            }

            read_bmp("aim.bmp",&data,&cols,&rows);                  // 이동하는 aim의 새로운 좌표를 업데이트하여 다시 출력
            fb_write_c(data,cols,rows,new_x, new_y);
            printf("x: %d       y: %d\n\n",new_y, new_x);          // aim의 실시간 좌표 확인
                
            if(new_y == 120 && new_x == 520){                           // 달고나 커팅 과정 중간지점 확인
                dalgona_start = 1; 
            }
            else if(new_y == 380 && new_x ==  520 && dalgona_start == 1){       
                printf("STAGE CLEAR\n\n");
                finish = 1;
                timer_end = 1;
                pwmSetPercent(100, 0);                                // 성공했을때 컬러 LED :: Blue
                pwmSetPercent(0, 1);
                pwmSetPercent(0, 2);
                read_bmp("victory1.bmp", &data, &cols, &rows);      // vicroty1.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
                sleep(2);
                read_bmp("victory2.bmp", &data, &cols, &rows);      // vicroty2.bmp 출력    (TFT LCD)
                fb_write(data, cols,rows);
            }
            if(new_y >= 120 && new_y <= 380 && new_x == 650){                           // ok :: 달고나를 올바르게 자르고 있는 과정
                printf("ok\n");                                                                                         // 선을 벗어나지 않음
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
            else{   
                printf("die !!! \n\n");                                                                         // die :: 달고나를 잘못 자름 => 게임 오버
                fail = 1;
                timer_end = 1;
                pwmSetPercent(0, 0);                                // 탈락했을때 컬러 LED :: Red
                pwmSetPercent(0, 1);
                pwmSetPercent(100, 2);
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

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pwmInactiveAll();
    buttonExit();
    ledLibExit();
    lcdtextExit();
    close_bmp();
    fb_close();   

    return 0;
}
