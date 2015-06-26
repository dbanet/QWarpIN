#ifndef WARCHIVE_H
#define WARCHIVE_H

#include "globals.h"
#include "ArchiveInterfaces/index.h"

#include <QObject>
#include <QFile>
#include <QPointer>

class WInstallationInformation{
public:
	explicit WInstallationInformation(QString packageID,QString reportScript):packageID(packageID),reportScript(reportScript){}
	QString packageID;
	QString reportScript;
};

class WArchive : public QObject
{
	Q_OBJECT
public:
	explicit WArchive(QFile *archive,QObject *parent = 0);
	QString name();
	QFile *file();
	void install(QString systemEnvironment);
	void test();

private:
	QPointer<WAbstractArchiveInterface> interface;

signals:

public slots:

};

#endif // WARCHIVE_H
