/*
EC-535 HomeWork-2   
Muhammad Kasim Patel
*/

#ifndef ACTORS_H_INCLUDED
#define ACTORS_H_INCLUDED

#include "fifo.h"

void     actor_fork(fifo_t *i1, fifo_t *o1, fifo_t *o2);
void     actor_increment(fifo_t *i1, fifo_t *o1);
void     actor_mul(fifo_t *i1, fifo_t *i2, fifo_t *q);
void     actor_print(fifo_t *i1);

#endif
