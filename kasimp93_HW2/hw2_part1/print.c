/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/


/* Print Actor */
/* Takes ones input and prints it */

#include <stdio.h>
#include <assert.h>
#include "fifo.h"

void actor_print(fifo_t *i1)
{
	assert(i1!=0);

	if(fifo_size(i1) > 0)
	{
		printf("%d\n",get_fifo(i1));
	}	 
}
