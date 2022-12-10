CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: libMyPeri.a btd_project

btd_project: btd_project.c libMyPeri.a led.h button.h buzzer.h fnd.h colorled.h textlcd.h libBitmap.h bitmapFileHeader.h
	$(CC) btd_project.c -lMyPeri -L. -lpthread -o btd_project

libMyPeri.a: button.o led.o buzzer.o fnd.o colorled.o textlcd.o libBitmap.o
	$(AR) rc libMyPeri.a led.o button.o buzzer.o fnd.o colorled.o textlcd.o libBitmap.o

libBitmap.o: libBitmap.h bitmapFileHeader.h libBitmap.c
	$(CC) -c libBitmap.c -o libBitmap.o

textlcd.o: textlcd.h textlcd.c
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
