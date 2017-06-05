/*
   EC-535 HomeWork 2
   Muhammad Kasim Patel
*/

#include <stdio.h>


#ifndef SIMULATOR_H
#define SIMULATOR_H

struct instruction
{
	int ln_num;
	char *opcode, *source, *destination;
};

struct instruction prog[50];

void processing(struct instruction program);
void traverse (int first, int last);

#endif
