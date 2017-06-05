/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/



/* Fork Actor */ 
/* takes one input and gives two copies of the input as the output */

#include <stdio.h>
#include <assert.h>
#include "fifo.h"

void actor_fork(fifo_t *i1, fifo_t *o1, fifo_t *o2)
{
	int a;
	assert(i1!=0);
	assert(o1!=0);
	assert(o2!=0);
	if(fifo_size(i1) > 0)
	{
		a = get_fifo(i1);		
		put_fifo(o1,a);	 
		put_fifo(o2,a);
	}	 
}
