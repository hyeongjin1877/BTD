CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a ledtest buttontest

ledtest: ledtest.c libMyPeri.a
	$(CC) ledtest.c -lMyPeri -L. -o ledtest

buttontest: buttontest.c libMyPeri.a
	$(CC) buttontest.c -lMyPeri -L. -lpthread -o buttontest

libMyPeri.a: button.o led.o
	$(AR) rc libMyPeri.a led.o button.o

button.o: button.h button.c
	$(CC) -c button.c -o button.o

led.o: led.h led.c
	$(CC) -c led.c -o led.o
