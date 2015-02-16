#ifndef WFILESYSTEMNODE_H
#define WFILESYSTEMNODE_H

#include "wfilesystem.h"

class WFileSystemNode : public QObject
{
    Q_OBJECT
public:
    explicit WFileSystemNode(QFile *file,QList<WFileSystemNode*> &children,QVariant userData=QVariant(),QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QVariant userData=QVariant(),QObject *parent=0);
    explicit WFileSystemNode(QFile *file,QVariant userData=QVariant(),QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QVariant userData=QVariant(),QObject *parent=0);
    WFileSystem::types type;
    QDir *dir;
    QFile *file;
    QString getNodeName();
    QList<QPointer<WFileSystemNode> > children;
    QVariant userData;
    void addChild(WFileSystemNode*);
    QString toJSON();
    ~WFileSystemNode();

};

#endif // WFILESYSTEMNODE_H
