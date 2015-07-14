#ifndef WCMDLINEPARSER_H
#define WCMDLINEPARSER_H
#include "globals.h"

#include <boost/program_options.hpp>

class WCmdLineParser{
public:
	WCmdLineParser(const QStringList &args);
	enum WCmdActions{
		NOP,INSTALL
	};
	struct WCmd{
		WCmdActions action;
	};
	WCmd parse();
	void displayHelp();
	void displayLicense();
	~WCmdLineParser();

private:
	int argc;
	char **argv;
	WCmd cmd;
	boost::program_options::options_description *options;

};

#endif // WCMDLINEPARSER_H
