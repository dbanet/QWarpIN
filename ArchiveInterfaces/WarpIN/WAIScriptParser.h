#ifndef WAISCRIPTPARSER_H
#define WAISCRIPTPARSER_H
#include "globals.h"
class WAIScriptParser
{
public:
    WAIScriptParser(QString WarpINScript);
    QString translate();

private:
    QString wpScript;
};

#endif // WAISCRIPTPARSER_H
