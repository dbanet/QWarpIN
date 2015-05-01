#ifndef WFILESYSTEMNODE_H
#define WFILESYSTEMNODE_H

#include "WFileSystem.h"

class WFileSystemNode : public QObject
{
    Q_OBJECT
public:
    explicit WFileSystemNode(WFile *file,QList<WFileSystemNode*> &children,QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QObject *parent=0);
    explicit WFileSystemNode(WFile *file,QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QObject *parent=0);
    WFileSystem::types type;
    QDir *dir;
    WFile *file;
    QString getNodeName();
    QList<QPointer<WFileSystemNode> > children;
    void addChild(WFileSystemNode*);
    QString toJSON();
    ~WFileSystemNode();

};

#endif // WFILESYSTEMNODE_H
