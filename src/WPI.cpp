#include "WPI.h"
#include "ui_WPI.h"
#include "WCmdLineParser.h"
#include "timestamp.h"

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
	qDebug()<<"Creating command line parser..."<<endl;
	auto cmdLnParser=WCmdLineParser(qApp->arguments());
	qDebug()<<"Entering command line parser..."<<endl;
	WCmdLineParser::WCmd cmd=cmdLnParser.parse();
	qDebug()<<"Command line parsing is done."<<endl;
	switch(cmd.action){
		case WCmdLineParser::NOP:
		break;
		case WCmdLineParser::INSTALL:
			if(0==cmd.args.count()){
				this->out<<endl;
				this->out<<"Please specify at least one installation subject."<<endl;
				cmdLnParser.displayHelpHint();
				this->out<<endl;
			} else try{
				this->arc=new WArchive(new QFile(cmd.args[0]));
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
