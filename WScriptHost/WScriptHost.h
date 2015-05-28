#ifndef WSCRIPTHOST_H
#define WSCRIPTHOST_H

#include "globals.h"
#include "../WArchive.h"
#include <QObject>
#include <QtScript/QtScript>
#define NGNEXC(engine) do{if(engine.hasUncaughtException()){qDebug()<<"Got script exception at " STRINGIZE(__FILE__) ":" STRINGIZE(__LINE__) ":" STRINGIZE(__PRETTY_FUNCTION__) "!"; qDebug()<<engine.uncaughtException().toString()<<" at script line number "<<engine.uncaughtExceptionLineNumber(); qDebug()<<"Exception backtrace: "<<engine.uncaughtExceptionBacktrace();}}while(false);
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
