CC = gcc
CFLAGS = -I/usr/local/include -L/usr/local/lib -lws2811 -lpigpio -lrt -lpthread -lm
SRC = main.c source/adc.c source/motor.c source/uart.c 
TARGET = main

all:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -f $(TARGET)
