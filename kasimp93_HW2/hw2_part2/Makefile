# -*- MakeFile -*-

all: simpleISS.o 
	gcc simpleISS.o -o simpleISS

main.o: main.c simulator.h
	gcc -c simpleISS.c

clean:
	rm -rf *.o simpleISS
