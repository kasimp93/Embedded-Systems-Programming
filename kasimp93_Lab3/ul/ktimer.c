/* EC-535 Lab 3 */
/* Muhammad Kasim Patel */
/* U75595108 */
/* User Level program */
/* ktimer.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

/*****************************************************************************************************
  Muhammad Kasim Patel
  
  Usage:
    ./ktimer [flag] [message]
	 
	-l (read)
	-s (write)
	
  Examples:
	./ktimer -l
		Print whatever message that the ktimer module is holding

	./ktimer -s Time line
		Creates a new timer with the name line and time Time, and updates if it already exists.

******************************************************************************************************/


void sighandler(int);
char line[128];

int main(int argc, char *argv[])
{
	char tmp[128];
	char flag[500];
	int pFile,oflags;
	unsigned long count;
	struct sigaction action, oa;
	
	//Open
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0)
	{
		fprintf (stderr, "ktimer module isn't loaded\n");
		return 1;
	}

	//Read from file
	if (argc == 2 && strcmp(argv[1], "-l") == 0)
	{
		char line_read[128]={0};
		read(pFile,line_read,128);
		printf("%s",line_read);
	}
	
	
	if (strcmp(argv[1], "-s") == 0) 
	{
		memset(&action, 0, sizeof(action));
		action.sa_handler = sighandler;
		action.sa_flags = SA_SIGINFO;
		sigemptyset(&action.sa_mask);
		sigaction(SIGIO, &action, NULL);
		fcntl(pFile, F_SETOWN, getpid());
		oflags = fcntl(pFile, F_GETFL);
		fcntl(pFile, F_SETFL, oflags | FASYNC);
		
		strcpy(tmp,argv[2]);
		strcat(tmp,argv[3]);
		strcpy(line,argv[3]);
		count = strlen(tmp);

		//Write
		write(pFile, tmp, count);
		
		//Read
		read(pFile,flag,500);

		if(strcmp(flag,"update")==0)
		{
			printf("Timer %s has been reset to %s seconds!\n",argv[3],argv[2]);
			exit(0);
		}
		else

		if(strcmp(flag,"exit")==0)
                {
                        printf("A timer already exists\n");
                        exit(0);
                }

		else
			pause();
	}
	close(pFile);
	return 0;
}

//SIGIO Handler
void sighandler(int signo)
{
	printf("%s\n",line);
}
