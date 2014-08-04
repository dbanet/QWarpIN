#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QFile>
#include <QDir>

class WFileSystemNode;
class WFileSystemTree;

class WFileSystemTree : public QObject
{
    Q_OBJECT
public:
    typedef enum{
        directory,file
    } types;
    explicit WFileSystemTree(WFileSystemNode *rootNode=0,QObject *parent=0);
    WFileSystemNode* rootNode();
    void setRootNode(WFileSystemNode*);
    ~WFileSystemTree();

private:
    QPointer<WFileSystemNode> root;

};

class WFileSystemNode : public QObject
{
    Q_OBJECT
public:
    explicit WFileSystemNode(QFile *file,QList<WFileSystemNode*> &children,QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QObject *parent=0);
    explicit WFileSystemNode(QFile *file,QObject *parent=0);
    explicit WFileSystemNode(QDir *dir,QObject *parent=0);
    WFileSystemTree::types type;
    QDir *dir;
    QFile *file;
    QList<QPointer<WFileSystemNode> > children;
    ~WFileSystemNode();

};

#endif // FILESYSTEMTREE_H
