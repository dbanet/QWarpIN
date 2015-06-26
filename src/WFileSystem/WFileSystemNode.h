#ifndef WFILESYSTEMNODE_H
#define WFILESYSTEMNODE_H

#include "WFileSystem.h"

class WFileSystemNode : public QObject
{
	Q_OBJECT
public:
	explicit WFileSystemNode(WFile *_file,QList<WFileSystemNode*> &children,QObject *parent=0);
	explicit WFileSystemNode(QDir *_dir,QList<WFileSystemNode*> &children,QObject *parent=0);
	explicit WFileSystemNode(WFile *_file,QObject *parent=0);
	explicit WFileSystemNode(QDir *_dir,QObject *parent=0);
	WFileSystem::types type;
	QList<QPointer<WFileSystemNode> > children;
	~WFileSystemNode();

public slots:
	QString nodeName();
	WFileSystemNode* navigate(QString path);
	void addChild(WFileSystemNode*);
	QString toJSON();
	QDir* dir();
	void setDir(QDir *dir);
	WFile* file();
	void setFile(WFile *file);

private:
	QDir *_dir;
	WFile *_file;

};

Q_DECLARE_METATYPE(WFileSystemNode*)

#endif // WFILESYSTEMNODE_H
