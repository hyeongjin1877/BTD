#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "temperature.h"

int main( int argc,  char **argv){ 
	
		char *buffer; int file;
		file=spi_init("/dev/spidev1.0"); 		//dev
		buffer=(char *)spi_read_lm74(file); 
		close(file);
		
		int value = 0; 											//13비트 Big Endian Signed Int를 16비트 Little Endian Signed Int로 바꾼다
		value = (buffer[1] >> 3); 							//Last 3bit: Trashes 날려버리는 함수: 남은 5비트만 value에 더해짐
		value += (buffer[0]) << 5; 						//위에서 5비트 더한거에 상위 8비트 (= 13비트) 를 원하는 값으로 채움
		if ( buffer[0]&0x80 ) 								//만약 buffer[0]의 최상위 8번째 비트가 1이라면 사실은 음수 = 영하 기온이 디텍팅 된 것
		{																	//그럼 value의 32번 비트에서부터 14번 비트까지 1로 채워 줘야 함 (2의 보수, 음수 참고)
				int i=0;
				for (i=31;i > 12; i--)
				value |= (1<<i);									 //1로 비트를 채움
		}
																			//다 바꿈
		
		double temp = (double)value*0.0625; 
																			//1비트가 0.0625도
		printf("Current Temp: %lf \n", temp);
		return 0;
}
