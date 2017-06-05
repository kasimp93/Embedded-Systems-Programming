/*
   EC-535 HomeWork 2
   Muhammad Kasim Patel
*/


#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"

int8_t R[6];
int source = 0;
int destination = 0;
int exec = 0;
int cycles_total = 0;
int8_t local_mem[256];
int memory_flags[256];
int st = 0;
int est = 0;	
int num = 0;
int mem_inst = 0;	
int local_hits = 0;	
int loop = 0;

/*Structure */

struct file_instructions
{
	int line;
	char instruction[10];
	char operand_1[10];
	char operand_2[10];
}file[100];



/*Main Function*/


void main(int argv, char *argc[])
{
	FILE *q;
	q = fopen(argc[1],"r");
	int last = 0;
	int i = 0;	
	char line[80];
	int first=0;

	while(fgets(line,80,q)!=NULL)
	{
		sscanf(line,"%d %st %[^','], %s",&file[num].line,file[num].instruction,file[num].operand_1,file[num].operand_2);
		num++;
	}
	last = num;
	decode(first,last);
	fclose(q);
	printf("Total number of instructions in the code: %d\n",num);
	printf("Total number of executed instructions: %d\n",exec);
	printf("Total number of clock cycles: %d cycles_total\n",cycles_total);
	printf("Number of hits to local memory: %d\n",local_hits);
	printf("Total number of execcuted LD/ST instructions: %d\n",mem_inst);
	return 0;
}


/*Decoding Each Instruction*/

void decode(int first, int last)
{
int i = 0;
int j = 0;
int flag = 0;
int address = 0;
	
for(i=first; i<=last; i++)
{
	if(strcmp(file[i].instruction, "MOV")==0)                                                                                                                                        
	{

		destination = atoi(&file[i].operand_1[1]);
		if(atoi(file[i].operand_2) == 0)
		{
			source = atoi(&file[i].operand_2[1]);
			R[destination-1] = R[source-1];
		}	
		else
		{
			source = atoi(file[i].operand_2);
			R[destination-1] = source;
		}
		
		cycles_total = cycles_total + 1;
		exec = exec + 1;
	}

	else if(strcmp(file[i].instruction, "ADD")==0)
	{ 

		destination = atoi(&file[i].operand_1[1]);
		if(atoi(file[i].operand_2) == 0)
		{
			source = atoi(&file[i].operand_2[1]);
			R[destination-1] = R[destination-1] + R[source-1];
		}	
		else
		{
			source = atoi(file[i].operand_2);
			R[destination-1] = R[destination-1] + source;
		}
		cycles_total = cycles_total + 1;
		exec = exec + 1;
	}
	
	else if(strcmp(file[i].instruction, "CMP")==0)
	{

		destination = atoi(&file[i].operand_1[1]);
		source = atoi(&file[i].operand_2[1]);
		if(R[destination-1] == R[source-1])
		{
			flag = 1;
		}
		else
		{
			flag = 0;
		}
		cycles_total = cycles_total + 2;
		exec = exec + 1;
	}	
		
        else
	if(strcmp(file[i].instruction, "JE")==0)
	{
		exec = exec + 1;
		cycles_total = cycles_total + 1;
		continue;	
	}
	
	else if(strcmp(file[i].instruction, "JMP")==0)
	{
		if(flag == 1)
			continue;
		else
		{
			exec = exec + 1;
			cycles_total = cycles_total + 1;
			est = i;
			destination = atoi(file[i].operand_1);
			for(j=0;j<i;j++)
			{
				if(file[j].line == destination)
					st = j;
			}
			decode(st,est);
		}
	}
	
	else if(strcmp(file[i].instruction, "ST")==0)
	{
		mem_inst = mem_inst + 1;
		exec = exec + 1;
		destination = atoi(&file[i].operand_1[2]);
		source = atoi(&file[i].operand_2[1]);
		address = R[destination-1];
		if(memory_flags[address]==1)
		{
			local_hits = local_hits + 1;
			local_mem[address] = R[source-1];
			cycles_total = cycles_total + 2;
		}
		else
		{
			memory_flags[address] = 1;
			local_mem[address] = R[source-1];
			cycles_total = cycles_total + 40;
		}
	}
	
	else if(strcmp(file[i].instruction, "LD")==0)
        {
		local_hits = local_hits + 1;
		mem_inst = mem_inst + 1;
                exec = exec + 1;
                destination = atoi(&file[i].operand_1[1]);
                source = atoi(&file[i].operand_2[2]);
                address = R[source-1];
                if(memory_flags[address]==1)
                {
                        R[destination-1] = local_mem[address];
                        cycles_total = cycles_total + 2;
                }
        }	
}
}
