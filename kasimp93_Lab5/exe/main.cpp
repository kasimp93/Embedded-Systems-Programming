#include "mytimer.h"
#include <QApplication>
#include <QCoreApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
     QApplication app(argc, argv);
    QWidget* window = new QWidget();
    QWidget* box = new QWidget();
    MyTimer* mytimer = new MyTimer(window);
    QGridLayout* layout = new QGridLayout(window);
    // Create MyTimer instance
    // QTimer object will be created in the MyTimer constructor
     QLabel label;
    label.setText("Printing Line 1\nPrinting Line 2\n");
    label.setAlignment(Qt::AlignCenter);
    layout->addWidget(&label, 0, 0);
    layout->addWidget(box, 1, 0);
    window->setLayout(layout);
    window->showFullScreen();
    window->show();
    

    QPalette red = box->palette();
    red.setColor(QPalette::Window, Qt::red);
    QPalette green = box->palette();
    green.setColor(QPalette::Window, Qt::green);
    QPalette yellow = box->palette();
    yellow.setColor(QPalette::Window, Qt::yellow);
    QPalette blue = box->palette();
    blue.setColor(QPalette::Window, Qt::blue);

    box->setAutoFillBackground(true);
 

    while(1){
	char temp[128];
        sprintf(temp, "%s", mytimer->buffer);
	printf("the temp is %s\n",temp);
	label.setText(temp);
	qApp->processEvents();
        usleep(100000);
    }

    return app.exec();
}

