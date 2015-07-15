#ifndef WCMDLINEPARSER_H
#define WCMDLINEPARSER_H
#include "globals.h"

#include <boost/program_options.hpp>
#include <boost/assign.hpp>
#include <QStringList>

class WCmdLineParser{
public:
	WCmdLineParser(const QStringList &args);
	enum WCmdActions{
		NOP,INSTALL
	};
	QMap<QString,WCmdActions> WCmdActionsStr=(
		static_cast<QMap<QString,WCmdActions> >(
			implicit_cast<std::map<QString,WCmdActions> >(
				boost::assign::map_list_of
					("NOP",NOP)
					("INSTALL",INSTALL)
			)
		)
	);
	struct WCmd{
		WCmdActions action;
		QStringList args;
		bool helpMode=false;
	};
	WCmd parse();
	void displayHelpHint();
	void displayHelp();
	void displayLicense();
	void displayVersionInformation();
	~WCmdLineParser();

private:
	int argc;
	char **argv;
	WCmd cmd;
	boost::program_options::options_description *userOptions;
	boost::program_options::options_description *hiddenOptions;
	boost::program_options::options_description *options;
	boost::program_options::positional_options_description *poptions;

};

#endif // WCMDLINEPARSER_H
