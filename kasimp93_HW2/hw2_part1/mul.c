/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/


/* Multiply Actor */
/* Takes two inputs, multiplies them and gives out the product as the output */

#include <stdio.h>
#include <assert.h>
#include "fifo.h"

void actor_mul(fifo_t *i1, fifo_t *i2, fifo_t *q)
{
	assert(i1!=0);
	assert(i2!=0);
	assert(q !=0);

	if ((fifo_size(i1) > 0) && (fifo_size(i2) > 0))
	{
	 put_fifo(q, get_fifo(i1) * get_fifo(i2));
	}
}
