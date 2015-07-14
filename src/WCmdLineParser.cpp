#include "WCmdLineParser.h"
#include <QStringList>
using namespace boost::program_options;
using namespace std;

WCmdLineParser::WCmdLineParser(const QStringList &args){
	this->argc=args.length();
	this->argv=new char*[this->argc];
	for(int i=0;i<this->argc;++i){
		this->argv[i]=new char[args[i].length()];
		strcpy(this->argv[i],args[i].toStdString().c_str());
	}

	this->options=new options_description("Options");
	this->options->add_options()
		("help,h","display help")
		("license","display licensing information")
	;
}

WCmdLineParser::WCmd WCmdLineParser::parse(){
	variables_map vm;
	store(parse_command_line(this->argc,this->argv,*this->options),vm);
	notify(vm);

	if(vm.count("help")){
		this->displayHelp();
		this->cmd.action=NOP;
	}
	if(vm.count("license")){
		this->displayLicense();
		this->cmd.action=NOP;
	}
	if(vm.count("install")){
		this->cmd.action=INSTALL;
	}
	return this->cmd;
}

void WCmdLineParser::displayHelp(){
	this->options->print(cout);
}

void WCmdLineParser::displayLicense(){
	cout<<loadModule(":/LICENSE").toStdString();
}

WCmdLineParser::~WCmdLineParser(){
	for(int i=0;i<this->argc;++i)
		delete this->argv[i];
	delete[] this->argv;
	delete this->options;
}
