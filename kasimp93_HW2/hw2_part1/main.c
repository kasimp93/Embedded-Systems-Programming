/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/


#include <stdio.h>
#include "fifo.h"
#include "actors.h"

int main()
{
	/*Creating different queues*/
	fifo_t fork1_input,increment_input,fork2_input;
	fifo_t fork1_output,increment_output,fork2_output, mul_output;
	int i=0;

	/*Initializing the queues*/
	init_fifo(&fork1_input);
	init_fifo(&fork2_input);
	init_fifo(&increment_input);
	init_fifo(&fork1_output);
	init_fifo(&fork2_output);
	init_fifo(&increment_output);

	/*Providing initial tokens*/
	put_fifo(&fork1_input, 42);
	put_fifo(&fork2_input, 1);
	for(i=0;i<100;i++)
	{
		actor_fork(&fork1_input, &fork1_output, &fork1_input);
		actor_fork(&fork2_input, &increment_input, &fork2_output);
		actor_increment(&increment_input, &fork2_input);
		actor_mul(&fork1_output, &fork2_output, &mul_output);
		actor_print(&mul_output);
	}
	return 0;
}
