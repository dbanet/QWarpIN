#include "globals.h"
#include "wpi.h"
#include <QApplication>
#include <QTimer>

void dbgout(QtMsgType type,const char* msg){
	fprintf(stderr,msg);
	fflush(stderr);
}

int main(int argc, char *argv[]){
	qDebug()<<"Starting up.";
	qInstallMsgHandler(dbgout);
	qDebug()<<"Debug output is on."<<endl;
	qDebug()<<"Creating QApplication...";
	QApplication a(argc, argv); qDebug()<<"done."<<endl;
	qDebug()<<"Creating WPI...";
	WPI wpi; qDebug()<<"done."<<endl;
	qDebug()<<"Setting up signals and slots...";
	QObject::connect(&a,SIGNAL(aboutToQuit()),&wpi,SLOT(aboutToQuit()));
	QTimer::singleShot(100,&wpi,SLOT(run())); qDebug()<<"done."<<endl;
	qDebug()<<"Entering main event loop."<<endl;
	int rc=a.exec();
	qDebug().nospace()<<"Exited main event loop with rc="<<rc<<"."<<endl;
	qDebug()<<"Quitting."<<endl;
	return rc;
}
