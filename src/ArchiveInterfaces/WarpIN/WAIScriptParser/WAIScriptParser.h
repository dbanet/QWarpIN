#ifndef WAISCRIPTPARSER_H
#define WAISCRIPTPARSER_H
#include "globals.h"
#include <QDomDocument>

class Parser;

class WAIScriptParser
{
public:
    WAIScriptParser(QString WarpINScript);
    QString translate();
    ~WAIScriptParser();

private:
    QString wpScript;
    std::istringstream *wpScriptStream;
    Parser *parser;

};

#endif // WAISCRIPTPARSER_H
