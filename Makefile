CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a ledtest buzzertest fndtest buttontest colorledtest textlcdtest gyrotest

gyrotest: gyrotest.c libMyPeri.a
	$(CC) gyrotest.c -lMyPeri -L. -o gyrotest

textlcdtest: textlcdtest.c libMyPeri.a
	$(CC) textlcdtest.c -lMyPeri -L. -o textlcdtest

colorledtest: colorledtest.c libMyPeri.a
	$(CC) colorledtest.c -lMyPeri -L. -o colorledtest

ledtest: ledtest.c libMyPeri.a
	$(CC) ledtest.c -lMyPeri -L. -o ledtest

buttontest: buttontest.c libMyPeri.a
	$(CC) buttontest.c -lMyPeri -L. -lpthread -o buttontest

buzzertest: buzzertest.c libMyPeri.a
	$(CC) buzzertest.c -lMyPeri -L. -o buzzertest

fndtest: fndtest.c libMyPeri.a
	$(CC) fndtest.c -lMyPeri -L. -o fndtest

libMyPeri.a: button.o led.o buzzer.o fnd.o colorled.o textlcd.o gyro.o
	$(AR) rc libMyPeri.a led.o button.o buzzer.o fnd.o colorled.o textlcd.o gyro.o

gyro.o: gyro.h gyro.c
	$(CC) -c gyro.c -o gyro.o

textlcd.o: textlcddrv.h textlcd.c
	$(CC) -c textlcd.c -o textlcd.o

colorled.o: colorled.h colorled.c
	$(CC) -c colorled.c -o colorled.o

fnd.o: fnd.h fnd.c
	$(CC) -c fnd.c -o fnd.o

buzzer.o: buzzer.h buzzer.c
	$(CC) -c buzzer.c -o buzzer.o

button.o: button.h button.c
	$(CC) -c button.c -o button.o

led.o: led.h led.c
	$(CC) -c led.c -o led.o
