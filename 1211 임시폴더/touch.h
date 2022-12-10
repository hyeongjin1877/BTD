#ifndef _TOUCH_H_
#define _TOUCH_H_
#define MESSAGE_ID_TOUCH 1138

typedef struct
{
	long int messageNum;
	int keyInput;
	int pressed;
	int x;
	int y;
} BUTTON_MSG_TOUCH;

int touchInit(void);
int touchExit(void);

#endif
