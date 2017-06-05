OBJ = main.o fftmisc.o  fourierf.o
FILE = main.c fftmisc.c  fourierf.c
CFLAGS = -static -O3 -g

fft: ${OBJ} Makefile
	/ad/eng/courses/ec/ec535/arm-linux/bin/arm-linux-gcc ${CFLAGS} ${OBJ} -o fft -lm -v
fftmisc.o: fftmisc.c
	/ad/eng/courses/ec/ec535/arm-linux/bin/arm-linux-gcc ${CFLAGS} -c fftmisc.c -v
fourierf.o: fourierf.c
	/ad/eng/courses/ec/ec535/arm-linux/bin/arm-linux-gcc ${CFLAGS} -c fourierf.c -v
main.o: main.c
	/ad/eng/courses/ec/ec535/arm-linux/bin/arm-linux-gcc ${CFLAGS} -c main.c -v

clean:
	rm -rf *.o fft output*
