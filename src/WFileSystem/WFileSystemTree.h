#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "WFileSystem.h"

class WFileSystemTree : public QObject
{
    Q_OBJECT
public:
    explicit WFileSystemTree(WFileSystemNode *rootNode=0,QObject *parent=0);
    ~WFileSystemTree();

public slots:
    WFileSystemNode* rootNode();
    WFileSystemNode* navigate(QString path);
    void setRootNode(WFileSystemNode*);
    void addChild(WFileSystemNode*);
    QString toJSON();

private:
    QPointer<WFileSystemNode> root;

};

Q_DECLARE_METATYPE(WFileSystemTree*)

#endif // FILESYSTEMTREE_H
