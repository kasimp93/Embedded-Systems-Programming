/*
   EC-535 HomeWork 1
   Muhammad Kasim Patel
*/

#include <stdio.h>
#include "bits.h"

int main(int argc, char *argv[])
{
  FILE *file1;
  FILE *file2;
  file1 = fopen(argv[1],"r");
  file2 = fopen(argv[2],"w");

    int i=0;
    int y=0;
    unsigned int x =0;
    unsigned int num=0;
    while(fscanf(file1, "%u", &num) != EOF) 
   {
	x = BinaryMirror(num);
	y = SequenceCount(num);  
	fprintf(file2, "%u %u\n", x,y);

        i++;
   }
    fclose(file1);
    fclose(file2);
  

return 0;
}
