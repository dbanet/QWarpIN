#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "WFileSystem.h"

class WFileSystemTree : public QObject
{
    Q_OBJECT
public:
    explicit WFileSystemTree(WFileSystemNode *rootNode=0,QObject *parent=0);
    WFileSystemNode* rootNode();
    void setRootNode(WFileSystemNode*);
    void addChild(WFileSystemNode*);
    QString toJSON();
    ~WFileSystemTree();

private:
    QPointer<WFileSystemNode> root;

};

#endif // FILESYSTEMTREE_H
