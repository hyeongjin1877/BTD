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
#define     INPUT_DEVICE_LIST   "/dev/input/event"      //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define    PROBE_FILE   "/proc/bus/input/devices"      //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일

#define HAVE_TO_FIND_1    "N: Name=\"WaveShare WaveShare Touchscreen\"\n"
#define HAVE_TO_FIND_2   "H: Handlers=mouse0 event4"



int probeTouchPath(char *newPath)
{
   int returnValue = 0;   //touch에 해당하는 event#을 찾았나?
   int number = 0;         //찾았다면 여기에 집어넣자
   FILE *fp = fopen(PROBE_FILE,"rt");   //파일을 열고

   while(!feof(fp))   //파일 끝까지 읽어들인다.
   {
      char tmpStr[200];  //200자를 읽을 수 있게 버퍼
      fgets(tmpStr,200,fp);   //최대 200자를 읽어봄
      //printf ("%s",tmpStr);
      if (strcmp(tmpStr,HAVE_TO_FIND_1) == 0)
      {
         printf("YES! I found!: %s\r\n", tmpStr);
         returnValue = 1;   //찾음
      }
      if ( (returnValue == 1) &&    //찾은 상태에서
       (strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0) ) //Event??을 찾았으면
      {   
         printf ("-->%s",tmpStr);         
         printf("\t%c\r\n",tmpStr[strlen(tmpStr)-3]);
         number = tmpStr[strlen(tmpStr)-3] - '0';   //Ascii character '0'-'9' (0x30-0x39) to interger(0)
      break; //while 문 탈출
      }
   }
   //이 상황에서 number에는 event? 중 ? 에 해당하는 숫자가 들어가 있다.
   fclose(fp);   
   if (returnValue == 1)
   sprintf (newPath,"%s%d",INPUT_DEVICE_LIST,number);
   //인자로 들어온 newPath 포인터에 
   //  /dev/input/event? 의 스트링을 채움
   return returnValue;
}

static char touchPath[200];
static int fd;
static int messageID;
static pthread_t touchTh_id;

static void *touchThFunc(void* arg)
{    
   BUTTON_MSG_T msgTx;
   msgTx.messageNum = 1;
   struct input_event stEvent;
   printf("Button Thread Ready\r\n");
   while (1)
   {
      read(fd, &stEvent, sizeof (stEvent));
      if(stEvent.type == EV_ABS)
      {
         //뭔가 좌표값이 들어올것만 같다
         if(stEvent.code == ABS_MT_POSITION_X)
         {
            printf("you touch X: %d\r\n", stEvent.value);
         }
         else if(stEvent.code == ABS_MT_POSITION_Y)
         {
            printf("you touch Y: %d\r\n", stEvent.value);
         }
      }
      else if(stEvent.type == EV_SYN)
      {
         //좌표 입력이 끝났다.
         printf("You finished Touch!\r\n");
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
   if (probeTouchPath(touchPath) == 0)
   {
      printf("Device Touch Open Failed\r\n");
      return 0;
   }
   fd=open (touchPath, O_RDONLY);
   messageID = msgget (MESSAGE_ID, IPC_CREAT|0666);
   pthread_create(&touchTh_id, NULL, touchThFunc, NULL);
   return 1;
}

int touchExit(void)
{
   pthread_cancel(touchTh_id);
   close(fd);
}
