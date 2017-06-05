#include "mytimer.h"
#include <QDebug>
#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QBoxLayout>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;



MyTimer::MyTimer(QWidget* window)
{
    // create a timer
    timer = new QTimer(this);
    mywindow = window;

    for(int i = 0; i < 128; i++)
        buffer[i] = 0;

    sprintf(buffer,"Value\tSpeed\tState\tDirection\tBrightness\n");


    // setup signal and slot
    connect(timer, SIGNAL(timeout()),
          this, SLOT(MyTimerSlot()));

    // msec
    timer->start(500);
}

void MyTimer::MyTimerSlot()
{
   // qDebug() << "Timer...";
	
   	char line[128];
	char temp;
  	int pFile;
	//int value, period, start_hold, up_down, brightness;
	int i=0;
	int number = 0;
	pFile = open("/dev/mygpio",O_RDONLY);	
	read(pFile,line,128);
	//printf("the line is %s\n",line);
	sprintf(buffer,"%s\n",line);	
	timer->start(500);
}

