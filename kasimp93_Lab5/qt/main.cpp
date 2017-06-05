#include <stdio.h>
#include <stdlib.h>
#include <QApplication>
#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QString>
#include <QWidget>
#include <QPalette>
#include <QLabel>
#include <QtGui>
#include <QFont>
#include <QPainter>

int main(int argc, char *argv[])
{
	FILE *pFile;
	size_t nread;
	char line[1024];
	char buf[1024];
	int i = 0;
	int spscount = 0;
	char fc = 'h';

	//QWidget w;
  

    QApplication app(argc, argv);
	QWidget widget;
	QPalette p = widget.palette();
	QVBoxLayout layout(&widget);
	QLabel label;
	label.setAlignment(Qt::AlignCenter);
	layout.addWidget(&label);
	QString display;
	widget.showFullScreen();

	// 1 1 up running low ; 11 1 up running low

	while(TRUE)
	{
		pFile = fopen("/dev/mygpio", "r");
		if(	fgets(buf,128,pFile) != NULL )
		{		
		    label.setText(buf);
		}

//   		label.show();
		for(i=0;i<strlen(buf);i++){
			if(buf[i] == ' ')
				spscount++;
			if(spscount > 3 ){
				fc = buf[i+1];
				spscount = 0;
			}
		}
		//printf("%c\n", fc);
		if(fc == 'h'){
			p.setColor(QPalette::Window, Qt::blue);
 			widget.setPalette(p);
  			widget.setAutoFillBackground(true);
		}
		else if(fc == 'm'){
			p.setColor(QPalette::Window, Qt::red);
 			widget.setPalette(p);
  			widget.setAutoFillBackground(true);
		}
		else if(fc == 'l'){
			p.setColor(QPalette::Window, Qt::green);
 			widget.setPalette(p);
  			widget.setAutoFillBackground(true);
		}
			
		strcpy(buf,"");
		fclose(pFile);
		qApp->processEvents();

	}


	return app.exec();

}

//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);
//    QLabel *label = new QLabel("Hello Qt!");
//    label->show();
//    return app.exec();
//}




