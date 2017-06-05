/*
   EC-535 HomeWork 1
   Muhammad Kasim Patel
*/


/* function returning the binary mirror  */


#include <stdio.h>
#include "bits.h"

unsigned int BinaryMirror(unsigned int n)

{
    unsigned int x[32];
    unsigned int remainder;
    unsigned int number;
    int i;
  
    for(i = 0 ; i <=31 ; i+=1) 
    {
        remainder = n%2;
        n = n/2;
	x[i] = remainder;
     }
	
     number = x[0];
     for(i = 0;i<=30; i+=1)
     { 
        number = (number*2) + x[i+1];    
      }

    return number;
}

/*_________________________________________________________________________*/

/* function returning the Sequence Count  */

unsigned int SequenceCount(unsigned int n)

/* local variable declaration */
   
{
    unsigned int x[32];
    unsigned int remainder;
    int i;
    unsigned int y=0;
  
    for(i = 31 ; i >=0 ; i-=1) 
    {
        remainder = n%2;
        n = n/2;
	x[i] = remainder;	    
     } 
          
	for(i = 0;i <= 30; i += 1)
     { 
	if ((x[i]==1) && (x[i+1]==0)) 
	{
	y+=1;
        }
          
      }

    return y;
} 
