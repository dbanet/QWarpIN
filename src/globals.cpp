#include "globals.h"
#include <QFile>

QString loadModule(QString path){
	QFile file;
	file.setFileName(path);
	file.open(QIODevice::ReadOnly);
	QString module=file.readAll();
	file.close();
	return module;
}
