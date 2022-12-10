#include "touch.h"
#include <sys/ipc.h>
#include <stdio.h>

int main(void)
{
    touchInit();
    
    int msgID = msgget(MESSAGE_ID, IPC_CREAT|0666);
    BUTTON_MSG_T recvMsg;
    while(1)
    {
        msgrcv(msgID, &recvMsg, sizeof(recvMsg)-sizeof(long int), 0, 0);

        switch(recvMsg.keyInput)
        {
            case 999:
            
                if( recvMsg.pressed == 1)
                {
                    if(recvMsg.x > 705) printf("NOPE\r\n");
                    else if (recvMsg.y > 400) printf("NOPE\r\n");
                    else if (recvMsg.x < 320) printf("NOPE\r\n");
                    //else if (recvMsg.y > 500) printf("NOPE\r\n");
                    else printf("START!\r\n");
                }
        }     
    }
}