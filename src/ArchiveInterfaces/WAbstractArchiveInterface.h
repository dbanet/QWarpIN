#ifndef WABSTRACTARCHIVEINTERFACE_H
#define WABSTRACTARCHIVEINTERFACE_H
#include <QObject>
#include <QFile>

#include "globals.h"
#include "../WFileSystem/WFileSystem.h"

class WAbstractArchiveInterface : public QObject
{
	Q_OBJECT
public:
	virtual QString id() const=0;
	virtual QString arcName() const=0;
	virtual QFile* arcFile() const=0;
	virtual WFileSystemTree* files()=0;
	virtual QString script()=0;

signals:

public slots:

};

#endif // WABSTRACTARCHIVEINTERFACE_H
