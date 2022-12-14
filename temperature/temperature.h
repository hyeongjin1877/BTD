#ifndef _TEM_H_
#define _TEM_H_
int spi_init(char filename[40]);
int spi_exit(void);
char * spi_read_lm74(int file);
int getTemperature(void);

#endif
