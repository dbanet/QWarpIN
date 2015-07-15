#include "WCmdLineParser.h"
using namespace boost::program_options;
using namespace std;

WCmdLineParser::WCmdLineParser(const QStringList &args){
	this->argc=args.length(); qDebug()<<"Got"<<this->argc<<"command line arguments."<<endl;
	this->argv=new char*[this->argc]; qDebug()<<"Preparing...";
	for(int i=0;i<this->argc;++i){
		this->argv[i]=new char[args[i].length()];
		strcpy(this->argv[i],args[i].toStdString().c_str());
	}
	qDebug()<<"done."<<endl;
	qDebug()<<"Populating options table...";
	this->userOptions=new options_description("Options");
	this->userOptions->add_options()
		("help,h","display context help")
		("license","display legal information")
		("version,v","display version information")
	;

	this->hiddenOptions=new options_description("Hidden ptions");
	this->hiddenOptions->add_options()
		("command",value<string>()->default_value("NOP","NOP"),"execution mode")
		("command-args",value<vector<string> >()->default_value(vector<string>(),""),"command arguments")
	;

	this->poptions=new positional_options_description();
	this->poptions->add("command",1);
	this->poptions->add("command-args",-1);

	this->options=new options_description();
	this->options->add(*this->userOptions).add(*this->hiddenOptions);
	qDebug()<<"done."<<endl;
}

WCmdLineParser::WCmd WCmdLineParser::parse(){
	try{
		variables_map vm; qDebug()<<"Parsing...";
		store(
			command_line_parser(this->argc,this->argv)
				.options(*this->options)
				.positional(*this->poptions)
				.run()
			,vm
		); notify(vm); qDebug()<<"done."<<endl;

		qDebug()<<"Populating command structure..."<<endl;
		if(vm.count("license")){
			qDebug()<<"Got option ``license''. Ignoring everything and displaying the license."<<endl;
			this->displayLicense();
			qDebug()<<"End of the license."<<endl;
			this->cmd.action=NOP;
		}
		else if(vm.count("version")){
			qDebug()<<"Got option ``version''. Ignoring everything and displaying the version information."<<endl;
			this->displayVersionInformation();
			qDebug()<<"End of the version information."<<endl;
			this->cmd.action=NOP;
		}
		else{
			QString cmdStr(vm["command"].as<string>().c_str());
			if(!this->WCmdActionsStr.contains(cmdStr.toUpper())){
				cout<<endl;
				cout<<"Unrecognized command ``"<<cmdStr.toStdString()<<"''."<<endl;
				cout<<"For help, please invoke ``"<<this->argv[0]<<" --help"<<"''."<<endl;
				cout<<endl;
				this->cmd.action=NOP;
			}
			else switch(this->WCmdActionsStr.value(cmdStr.toUpper())){
				case NOP:
					qDebug()<<"Got command ``NOP''."<<endl;
					this->cmd.action=NOP;
				break;
				case INSTALL:
					qDebug()<<"Got command ``install''."<<endl;
					this->cmd.action=INSTALL;
					auto args=QVector<string>::fromStdVector(vm["command-args"].as<vector<string> >());
					for(auto it=args.begin();it!=args.end();++it) this->cmd.args<<QString((*it).c_str());
					qDebug()<<"Got"<<this->cmd.args.count()<<"install arguments."<<endl;
				break;
			}
		}
		/* are actually in help mode? */
		if(vm.count("help")){
			qDebug()<<"Got option ``help''. Entering context help mode."<<endl;
			this->displayHelp();
			qDebug()<<"End of the help screen."<<endl;
			this->cmd.helpMode=true;
		}
		qDebug()<<"Command structure is ready."<<endl;
	} catch(exception &e){
		qDebug()<<"done."<<endl;
		cout<<endl;
		cout<<"Error on command line: "<<e.what()<<endl;
		cout<<"For help, please invoke ``"<<this->argv[0]<<" --help"<<"''."<<endl;
		cout<<endl;
		this->cmd.action=NOP;
	}
	return this->cmd;
}

void WCmdLineParser::displayHelpHint(){
	cout<<"For help, please invoke ``"<<this->argv[0]<<" --help"<<"''."<<endl;
}

void WCmdLineParser::displayHelp(){
	this->userOptions->print(cout);
}

void WCmdLineParser::displayLicense(){
	cout<<loadModule(":/LICENSE").toStdString();
}

void WCmdLineParser::displayVersionInformation(){
	cout<<"TODO: Write version information text. Include GitHub and homepage/forums link."<<endl;
}

WCmdLineParser::~WCmdLineParser(){
	for(int i=0;i<this->argc;++i)
		delete this->argv[i];
	delete[] this->argv;
	delete this->userOptions;
	delete this->hiddenOptions;
	delete this->poptions;
	delete this->options;
}
