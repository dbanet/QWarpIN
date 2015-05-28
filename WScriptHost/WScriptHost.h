#ifndef WSCRIPTHOST_H
#define WSCRIPTHOST_H

#include "globals.h"
#include "../WArchive.h"
#include <QObject>
#include <QtScript/QtScript>

class WScriptHost : public QObject
{
    Q_OBJECT
public:
    explicit WScriptHost(QObject *parent = 0);
    void setInstallationContext(QString script,WFileSystemTree* files,QString systemEnvironment);
    WInstallationInformation install();

signals:

public slots:

private:
    QScriptEngine engine;
    static QScriptValue log(QScriptContext *context,QScriptEngine *engine);
    enum {
        freestandingContext,installationContext,responseContext
    } context;
    QString script;
    WFileSystemTree* files;
    QString systemEnvironment;
    QScriptValue installationObject;
    QScriptValue pkgInfo;

};

#endif // WSCRIPTHOST_H
