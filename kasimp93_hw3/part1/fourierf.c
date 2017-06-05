/*============================================================================

    fourierf.c  -  Don Cross <dcross@intersrv.com>

    http://www.intersrv.com/~dcross/fft.html

    Contains definitions for doing Fourier transforms
    and inverse Fourier transforms.

    This module performs operations on arrays of 'float'.

    Revision history:

1998 September 19 [Don Cross]
    Updated coding standards.
    Improved efficiency of trig calculations.

============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "fourier.h"
#include "ddcmath.h"

#define CHECKPOINTER(p)  CheckPointer(p,#p)

static void CheckPointer ( void *p, char *name )
{
    if ( p == NULL )
    {
        fprintf ( stderr, "Error in fft_float():  %s == NULL\n", name );
        exit(1);
    }
}



void fft_float (
    unsigned  NumSamples,
    int       InverseTransform,
    float    *RealIn,
    float    *ImagIn,
    float    *RealOut,
    float    *ImagOut )
{
    unsigned NumBits;    /* Number of bits needed to store indices */
    unsigned i, j, k, n;
    unsigned BlockSize, BlockEnd;

    double angle_numerator = 2.0 * DDC_PI;
    double tr, ti;     /* temp real, temp imaginary */

    if ( !IsPowerOfTwo(NumSamples) )
    {
        fprintf (
            stderr,
            "Error in fft():  NumSamples=%u is not power of two\n",
            NumSamples );

        exit(1);
    }

    if ( InverseTransform )
        angle_numerator = -angle_numerator;

    CHECKPOINTER ( RealIn );
    CHECKPOINTER ( RealOut );
    CHECKPOINTER ( ImagOut );

    NumBits = NumberOfBitsNeeded ( NumSamples );

    /*
    **   Do simultaneous data copy and bit-reversal ordering into outputs...
    */
    // Unrolled the first for loop, inlined and unrolled the reversebit function.
	
    unsigned p, rev,rev1,rev2,rev3,rev4,rev5,rev6,rev7,index,index1,index2,index3,index4,index5,index6,index7;
    for ( i=0; i < NumSamples; i+=4 )
    {
	index = i;
	index1 = i+1;
	index2 = i+2;
	index3 = i+3;
    	for ( p=rev=rev1=rev2=rev3=0; p < NumBits; p+=5 )
    	{
        	rev = (rev << 1) | (index & 1);
        	rev1 = (rev1 << 1) | (index1 & 1);
        	rev2 = (rev2 << 1) | (index2 & 1);
        	rev3 = (rev3 << 1) | (index3 & 1);
        	index >>= 1;
        	index1 >>= 1;
        	index2 >>= 1;
        	index3 >>= 1;
        	rev = (rev << 1) | (index & 1);
        	rev1 = (rev1 << 1) | (index1 & 1);
        	rev2 = (rev2 << 1) | (index2 & 1);
        	rev3 = (rev3 << 1) | (index3 & 1);
        	index >>= 1;
        	index1 >>= 1;
        	index2 >>= 1;
        	index3 >>= 1;
        	rev = (rev << 1) | (index & 1);
        	rev1 = (rev1 << 1) | (index1 & 1);
        	rev2 = (rev2 << 1) | (index2 & 1);
        	rev3 = (rev3 << 1) | (index3 & 1);
        	index >>= 1;
        	index1 >>= 1;
        	index2 >>= 1;
        	index3 >>= 1;
        	rev = (rev << 1) | (index & 1);
        	rev1 = (rev1 << 1) | (index1 & 1);
        	rev2 = (rev2 << 1) | (index2 & 1);
        	rev3 = (rev3 << 1) | (index3 & 1);
        	index >>= 1;
        	index1 >>= 1;
        	index2 >>= 1;
        	index3 >>= 1;
        	rev = (rev << 1) | (index & 1);
        	rev1 = (rev1 << 1) | (index1 & 1);
        	rev2 = (rev2 << 1) | (index2 & 1);
        	rev3 = (rev3 << 1) | (index3 & 1);
        	index >>= 1;
        	index1 >>= 1;
        	index2 >>= 1;
        	index3 >>= 1;
    	}
        RealOut[rev] = RealIn[i];
        RealOut[rev1] = RealIn[i+1];
        RealOut[rev2] = RealIn[i+2];
        RealOut[rev3] = RealIn[i+3];
		
	/* Used If Else instead of Conditional Statement and loop unrolling*/
	if(ImagIn == NULL){
		ImagOut[rev]= 0.0;
		ImagOut[rev1]= 0.0;
		ImagOut[rev2]= 0.0;
		ImagOut[rev3]= 0.0;}
	else{
		ImagOut[rev]= ImagIn[i];
		ImagOut[rev1]= ImagIn[i+1];
		ImagOut[rev2]= ImagIn[i+2];
		ImagOut[rev3]= ImagIn[i+3];}
    }

    /*
    **   Do the FFT itself...
    */

    BlockEnd = 1;
    for ( BlockSize = 2; BlockSize <= NumSamples; BlockSize <<= 1 )
    {
        double delta_angle = angle_numerator / (double)BlockSize;
        double sm2 = sin ( -2 * delta_angle );
        double sm1 = sin ( -delta_angle );
        double cm2 = cos ( -2 * delta_angle );
        double cm1 = cos ( -delta_angle );
        double w = 2 * cm1;
	double ar[3], ai[3];
        double temp;

        for ( i=0; i < NumSamples; i += BlockSize )
        {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {
                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0]*RealOut[k] - ai[0]*ImagOut[k];
                ti = ar[0]*ImagOut[k] + ai[0]*RealOut[k];

                RealOut[k] = RealOut[j] - tr;
                ImagOut[k] = ImagOut[j] - ti;

                RealOut[j] += tr;
                ImagOut[j] += ti;
            }
        }

        BlockEnd = BlockSize;
    }

    /*
    **   Need to normalize if inverse transform...
    */
	
	//Declared before the loop to speed up the code
	unsigned denom = NumSamples;
	
    if ( InverseTransform )
    {
        //double denom = (double)NumSamples;
		
		//used loop unrolling
        for ( i=0; i < NumSamples; i+=4 )
        {
            RealOut[i] /= denom;
            RealOut[i+1] /= denom;
            RealOut[i+2] /= denom;
            RealOut[i+3] /= denom;
            ImagOut[i] /= denom;
            ImagOut[i+1] /= denom;
            ImagOut[i+2] /= denom;
            ImagOut[i+3] /= denom;
        }
    }
}


/*--- end of file fourierf.c ---*/
