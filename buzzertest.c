#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

int main(int argc , char **argv)
{
	int freIndex;

	if (argc < 2 || findBuzzerSysPath() )
	{
		printf("Error!\n");
		doHelp();
		return 1;
	}

	freIndex = atoi(argv[1]);
	printf("freIndex :%d \n",freIndex);

	buzzerInit();
	
	if ( freIndex == 0)
	{
		buzzerStopSong();
	}
	else
	{
		buzzerPlaySong(freIndex);
	}
	
	return 0;
}
