/* EC-535 Lab 2 */
/* Muhammad Kasim Patel */
/* U75595108 */
/* User Level program */
/* ktimer.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************  
  Usage:
    ./ktimer [flag] [message]
	 
	-l (read)
	-s (write)
	
  Examples:
	./ktimer -l
		Print whatever message that the ktimer module is holding

	./ktimer -s ThisIsAMessage
		Write the string "ThisIsAMessage" to the ktimer module
	
******************************************************/

void printManPage(void);

int main(int argc, char *argv[]) {
	char line[256];
	int i = 0; 
	int count = 0;
	char temp[256];
	
	
	/* Check to see if the ktimer successfully has mknod run
	   Assumes that ktimer is tied to /dev/ktimer */
	FILE * pFile;
	
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	//Check if in read mode
	if (argc == 2 && strcmp(argv[1], "-l") == 0) 
	{
		pFile = fopen("/dev/mytimer", "r");
		while(fgets(line, 256, pFile) != NULL) 
		{
			printf("%s", line);
		}
		fclose(pFile);
	}
	else if (argc == 3 && strcmp(argv[1], "-l") == 0)
	{	
		pFile = fopen("/dev/mytimer", "w");
		fputs(argv[2],pFile);
		fclose(pFile);
		pFile = fopen("/dev/mytimer", "r");
		while (fgets(line,256,pFile) != NULL)
		{
                    	printf("%s", line);
		}
		fclose(pFile);
	}
	
	// Check if in write mode
	else if (strcmp(argv[1], "-s") == 0) 
	{
		pFile = fopen("/dev/mytimer", "w");
		strcpy(temp,argv[2]);
		strcat(temp,argv[3]);
		fputs(temp,pFile);
		fclose(pFile);
	}

	// Otherwise invalid
	else {
		printManPage();
	}

	return 0;
}


void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: read from the mytimer module\n");	
	printf(" -s: write [message] to the mytimer module\n");
}
