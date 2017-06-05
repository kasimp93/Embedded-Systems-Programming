# -*- MakeFile -*-

all: myBitApp
	
myBitApp: main.o bits.o
	gcc main.o bits.o -o myBitApp

main.o: main.c bits.h
	gcc -c main.c

bits.o: bits.c bits.h
	gcc -c bits.c

clean:
	rm -f *.o myBitApp 
