#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <pthread.h>

#include "textlcddrv.h"

stTextLCD stlcd;
static int fd;
static int len;
static unsigned int linenum = 0;

int lcdtextwrite(char *str1, char *str2, int lineFlag)  // 1번째, 2번째, 줄
{
        if (lineFlag == 1){
                stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
                len = strlen(str1);
        }
        else if (lineFlag == 2){
                stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
                len = strlen(str2);
        }
        else{
         printf("lineFlag :%d wrong. range(1~2)\n",lineFlag);
         return 1;}

        if (lineFlag == 1){
            if(len > COLUMN_NUM )
            memcpy(stlcd.TextData[stlcd.cmdData -1],str1,COLUMN_NUM);
            else
            memcpy(stlcd.TextData[stlcd.cmdData -1],str1,len);
        }
        else if(lineFlag == 2){
            if(len > COLUMN_NUM )
            memcpy(stlcd.TextData[stlcd.cmdData -1],str2,COLUMN_NUM);
            else
            memcpy(stlcd.TextData[stlcd.cmdData -1],str2,len);
        }

        stlcd.cmd = CMD_WRITE_STRING;
       
       
        fd = open(TEXTLCD_DRIVER_NAME,O_RDWR); // open driver
        if ( fd < 0 ) {
           perror("driver (//dev//peritextlcd) open error.\n");
           return 1;
           }
        
        write(fd,&stlcd,sizeof(stTextLCD));
        close(fd);

        return 0;
}