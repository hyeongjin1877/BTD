#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define ACCELPATH "/sys/class/misc/FreescaleAccelerometer/"
#define MAGNEPATH "/sys/class/misc/FreescaleMagnetometer/"
#define GYROPATH "/sys/class/misc/FreescaleGyroscope/"

static int new_x, new_y;

int get_accel_x(){
     int fd = 0;
    FILE *fp = NULL;

    fd = open (ACCELPATH "enable", O_WRONLY);
    dprintf(fd,"1");
    close(fd);
    fp = fopen(ACCELPATH "data", "rt");
    int accel[3];
    fscanf(fp, "%d, %d, %d", &accel[0], &accel[1], &accel[2]);
    new_x = accel[0];

    //printf("I read Accel %d, %d, %d\r\n", accel[0], accel[1], accel[2]);
    fclose(fp);

    return new_x;
}

int get_accel_y(){
     int fd = 0;
    FILE *fp = NULL;

    fd = open (ACCELPATH "enable", O_WRONLY);
    dprintf(fd,"1");
    close(fd);
    fp = fopen(ACCELPATH "data", "rt");
    int accel[3];
    fscanf(fp, "%d, %d, %d", &accel[0], &accel[1], &accel[2]);
    new_y = accel[1];

    //printf("I read Accel %d, %d, %d\r\n", accel[0], accel[1], accel[2]);
    fclose(fp);

    return new_y;
}

int gyro_options(void){
    int fd = 0;
    FILE *fp = NULL;

    //Accelerometer
    fd = open (ACCELPATH "enable", O_WRONLY);
    dprintf(fd,"1");
    close(fd);
    fp = fopen(ACCELPATH "data", "rt");
    int accel[3];
    fscanf(fp, "%d, %d, %d", &accel[0], &accel[1], &accel[2]);
    printf("I read Accel %d, %d, %d\r\n", accel[0], accel[1], accel[2]);
    fclose(fp);

    //Magnetometer
    fd = open (MAGNEPATH "enable", O_WRONLY);
    dprintf(fd,"1");
    close(fd);
    fp = fopen(MAGNEPATH "data", "rt");
    int magne[3];
    fscanf(fp, "%d, %d, %d", &magne[0], &magne[1], &magne[2]);
    printf("I read Magneto %d, %d, %d\r\n", magne[0], magne[1], magne[2]);
    fclose(fp);

     //Gyroscope
    fd = open (GYROPATH "enable", O_WRONLY);
    dprintf(fd,"1");
    close(fd);
    fp = fopen(GYROPATH "data", "rt");
    int gyro[3];
    fscanf(fp, "%d, %d, %d", &gyro[0], &gyro[1], &gyro[2]);
    printf("I read Gyroscope %d, %d, %d\r\n", gyro[0], gyro[1], gyro[2]);
    fclose(fp);
}