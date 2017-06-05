/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/

/* Increment Actor */ 
/* takes one input, increments the input and gives it as the output */


#include <stdio.h>
#include <assert.h>
#include "fifo.h"

void actor_increment(fifo_t *i1, fifo_t *o1)
{
	assert(i1!=0);
	assert(o1!=0);

	if(fifo_size(i1) > 0)
	{
		put_fifo(o1,get_fifo(i1)+1);	 
	}	 
}
