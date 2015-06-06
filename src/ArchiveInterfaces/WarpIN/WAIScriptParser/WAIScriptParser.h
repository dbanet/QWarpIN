#ifndef WAISCRIPTPARSER_H
#define WAISCRIPTPARSER_H
#include "globals.h"
#include <QDomDocument>

class WAIScriptParser
{
public:
    WAIScriptParser(QString WarpINScript);
    QString translate();

private:
    QString wpScript;
    QDomDocument dom;
    QDomElement wp;
    QDomElement body;

};

#endif // WAISCRIPTPARSER_H
