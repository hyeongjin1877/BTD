CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a ledtest

libMyPeri.a : led.c
	$(CC) led.c -c -o led.o
	$(AR) rc libMyPeri.a led.o

ledtest: ledtest.c
	$(CC) ledtest.c -lMyPeri -L. -o ledtest

clean:
	rm *.o -rf
	rm *.a -rf
	rm ledtest
