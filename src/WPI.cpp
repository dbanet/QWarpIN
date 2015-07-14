#include "WPI.h"
#include "ui_WPI.h"
#include "WCmdLineParser.h"

WPI::WPI(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WPI),
	out(stdout){
	ui->setupUi(this);
}

WPI::~WPI(){
	delete ui;
}

void WPI::on_actionTest_triggered(){
	this->arc->test();
}

void WPI::run(){
	this->out<<"WPI version "<<QWP_VERSION<<" (built "_TIMEZ_")"<<endl;
	this->out<<"Copyright (c) 2014, 2015 dbanet"<<endl;
	WCmdLineParser::WCmd cmd=WCmdLineParser(qApp->arguments()).parse();
	switch(cmd.action){
		case WCmdLineParser::NOP:
		break;
		case WCmdLineParser::INSTALL:
			try{
				this->arc=new WArchive(new QFile("cppcheck-1.66-dev-os2.wpi"));
				this->arc->install("{test:\"yay!\"}");
			} catch(exception &e){
				out<<e.what()<<endl;
				this->quit(1);
				return;
			}
		break;
	}
	this->quit(0);
	return;
}

void WPI::aboutToQuit(){

}

void WPI::quit(int rc){
	qApp->exit(rc);
	return;
}
