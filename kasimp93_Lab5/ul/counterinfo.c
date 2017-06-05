#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(int argc, char **argv)
{
	char line[128];
	FILE *pFile;
	int time;
	if (argc==2)
	{
//		pFile = fopen("/dev/mygpio","r");	
//		time = atoi(argv[1]);
        int speed = argv[1][0] -'0' ;		
		printf("Value	Speed	State	Direction	Brightness\n");
		while(1)
		{
            int pFile;
	   	 	char line[128];
//			system("cat /dev/mygpio");
	   	 	pFile = open("/dev/mygpio", "r");
            		read(pFile,line,128);
	    		printf("%s", line);
            		sleep(speed) ;
		}
		fclose(pFile);
	}
	else
		printf("Enter The Correct Number Of Arguments");
	return 0;
}


