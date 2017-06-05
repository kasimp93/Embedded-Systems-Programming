CC=/ad/eng/courses/ec/ec535/arm-linux/bin/arm-linux-gcc
CFLAGS=-Wall
ktimer: ktimer.o 
		$(CC) -o ktimer ktimer.o

ktimer.o: ktimer.c
		$(CC) -c ktimer.c

clean:
	rm ktimer ktimer.o
