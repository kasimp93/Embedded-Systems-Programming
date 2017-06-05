# -*- MakeFile -*-

all: main.o fork.o mul.o increment.o print.o fifo.o
	gcc main.o fork.o mul.o increment.o print.o fifo.o -o twentyfour

main.o: main.c fork.o mul.o increment.o print.o fifo.o fifo.h actors.h
	gcc -c main.c 

fork.o: fork.c fifo.o fifo.h
	gcc -c fork.c

increment.o: increment.c fifo.o fifo.h
	gcc -c increment.c

mul.o: mul.c fifo.o fifo.h
	gcc -c mul.c

print.o: print.c fifo.o fifo.h
	gcc -c print.c

fifo.o:	fifo.c fifo.h 
	gcc -c fifo.c

clean: 
	rm -rf *.o twentyfour
