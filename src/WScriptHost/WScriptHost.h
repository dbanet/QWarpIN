#ifndef WSCRIPTHOST_H
#define WSCRIPTHOST_H

#include "globals.h"
#include "../WArchive.h"
#include <QObject>
#include <QtScript/QtScript>
#define NGNEXC(engine) do{if(engine.hasUncaughtException()){qDebug()<<"Got script exception at " STRINGIZE(__FILE__) ":" STRINGIZE(__LINE__) ":" STRINGIZE(__PRETTY_FUNCTION__) "!"<<endl; qDebug()<<engine.uncaughtException().toString()<<" at script line number "<<engine.uncaughtExceptionLineNumber(); qDebug()<<"Exception backtrace: "<<engine.uncaughtExceptionBacktrace()<<endl;}}while(false);

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

	/* conversion functions */
	static QScriptValue WFileToScriptValue(QScriptEngine *engine,WFile* const &in);
	static void WFileFromScriptValue(const QScriptValue &object,WFile* &out);
	static QScriptValue WFileSystemTreeToScriptValue(QScriptEngine *engine,WFileSystemTree* const &in);
	static void WFileSystemTreeFromScriptValue(const QScriptValue &object,WFileSystemTree* &out);
	static QScriptValue WFileSystemNodeToScriptValue(QScriptEngine *engine,WFileSystemNode* const &in);
	static void WFileSystemNodeFromScriptValue(const QScriptValue &object,WFileSystemNode* &out);

};

#endif // WSCRIPTHOST_H
