#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>
#include "touch.h"

// first read input device
#define     INPUT_DEVICE_LIST_TOUCH   "/dev/input/event"      //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define    PROBE_FILE_TOUCH   "/proc/bus/input/devices"      //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일

#define HAVE_TO_FIND_1_TOUCH    "N: Name=\"WaveShare WaveShare Touchscreen\"\n"
#define HAVE_TO_FIND_2_TOUCH   "H: Handlers=mouse0 even"

int probeTouchPath(char *newPath)
{
   int returnValue_TOUCH = 0;   //touch에 해당하는 event#을 찾았나?
   int number_TOUCH = 0;         //찾았다면 여기에 집어넣자
   FILE *fp_TOUCH = fopen(PROBE_FILE_TOUCH,"rt");   //파일을 열고

   while(!feof(fp_TOUCH))   //파일 끝까지 읽어들인다.
   {
      char tmpStr_TOUCH[200];  //200자를 읽을 수 있게 버퍼
      fgets(tmpStr_TOUCH,200,fp_TOUCH);   //최대 200자를 읽어봄
      printf ("%s",tmpStr_TOUCH);
      if (strcmp(tmpStr_TOUCH,HAVE_TO_FIND_1_TOUCH) == 0)
      {
         printf("YES! I found!: %s\r\n", tmpStr_TOUCH);
         returnValue_TOUCH = 1;   //찾음
      }
      if ( (returnValue_TOUCH == 1) && (strncasecmp(tmpStr_TOUCH, HAVE_TO_FIND_2_TOUCH, strlen(HAVE_TO_FIND_2_TOUCH)) == 0) ) //Event??을 찾았으면
      {   
         printf ("-->%s",tmpStr_TOUCH);         
         printf("\t%c\r\n",tmpStr_TOUCH[strlen(tmpStr_TOUCH)-3]);
         number_TOUCH = tmpStr_TOUCH[strlen(tmpStr_TOUCH)-3] - '0';   //Ascii character '0'-'9' (0x30-0x39) to interger(0)
      break; //while 문 탈출
      }
      else printf("sibal");    
   }
   //이 상황에서 number에는 event? 중 ? 에 해당하는 숫자가 들어가 있다.
   fclose(fp_TOUCH);   
   if (returnValue_TOUCH == 1)
   sprintf (newPath,"%s%d",INPUT_DEVICE_LIST_TOUCH,number_TOUCH);
   //인자로 들어온 newPath 포인터에 
   //  /dev/input/event? 의 스트링을 채움
   return returnValue_TOUCH;
}

static char touchPath_TOUCH[200];
static int fd_TOUCH;
static int msgID_TOUCH;
static pthread_t touchTh_id_TOUCH;

static void *touchThFunc(void* arg)
{    
   int x_TOUCH = 0;
   int y_TOUCH = 0;

   struct input_event stEvent_TOUCH;
   TOUCH_MSG_T sendMsg_TOUCH;
   sendMsg_TOUCH.messageNum = 1;
   sendMsg_TOUCH.keyInput = 999;

   //printf("Touch Thread Ready\r\n");
   
   while (1)
   {
      read(fd_TOUCH, &stEvent_TOUCH, sizeof (stEvent_TOUCH));
      if(stEvent_TOUCH.type == EV_ABS)
      {
         //뭔가 좌표값이 들어올것만 같다
         if(stEvent_TOUCH.code == ABS_MT_POSITION_X)
         {
            x_TOUCH = stEvent_TOUCH.value;
            //printf("you touch X: %d\r\n", stEvent.value);
         }
         else if(stEvent_TOUCH.code == ABS_MT_POSITION_Y)
         {
            y_TOUCH = stEvent_TOUCH.value;
            //printf("you touch Y: %d\r\n", stEvent.value);
         }
      }
      else if((stEvent_TOUCH.type == EV_KEY) && (stEvent_TOUCH.code == BTN_TOUCH))
      {
         sendMsg_TOUCH.x = x_TOUCH;
         sendMsg_TOUCH.y = y_TOUCH;
         //좌표 입력이 끝났다.
         if(stEvent_TOUCH.value == 0)
         {
            sendMsg_TOUCH.pressed = 0;
            //printf("You finished Touch!\r\n");
         }
         else if (stEvent_TOUCH.value == 1)
         {
            sendMsg_TOUCH.pressed = 1;
            //printf("You touch Now!\r\n");
         }
         msgsnd(msgID_TOUCH, &sendMsg_TOUCH, sizeof(TOUCH_MSG_T) - sizeof(long int), 0);
      }
/*
      if ( ( stEvent.type == EV_KEY) )
      {
         msgTx.keyInput = stEvent.code;
         msgTx.pressed = stEvent.value;
         msgsnd(messageID, &msgTx, sizeof(msgTx) - sizeof(long int), 0);
      }*/
    }
}

int touchInit(void)
{
   if (probeTouchPath(touchPath_TOUCH) == 0)
   {
      printf("Device Touch Open Failed\r\n");
      return 0;
   }
   fd_TOUCH=open (touchPath_TOUCH, O_RDONLY);
   msgID_TOUCH = msgget (MESSAGE_ID_TOUCH, IPC_CREAT|0666);
   pthread_create(&touchTh_id_TOUCH, NULL, touchThFunc, NULL);
   return 1;
}

int touchExit(void)
{
   pthread_cancel(touchTh_id_TOUCH);
   close(fd_TOUCH);
}
