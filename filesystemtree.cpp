#include "filesystemtree.h"

WFileSystemTree::WFileSystemTree(WFileSystemNode *rootNode,QObject *parent) :
    QObject(parent){
    this->root=rootNode;
}

WFileSystemNode* WFileSystemTree::rootNode(){
    return this->root;
}

void WFileSystemTree::setRootNode(WFileSystemNode* rootNode){
    this->root=rootNode;
}

WFileSystemTree::~WFileSystemTree(){
    delete this->root;
}

WFileSystemNode::WFileSystemNode(QFile *file,QList<WFileSystemNode*> &children,QObject *parent) :
    QObject(parent),
    file(file){
    foreach(WFileSystemNode *node,children)
        this->children.append(QPointer<WFileSystemNode>(node));
    this->type=WFileSystemTree::file;
}

WFileSystemNode::WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QObject *parent) :
    QObject(parent),
    dir(dir){
    foreach(WFileSystemNode *node,children)
        this->children.append(QPointer<WFileSystemNode>(node));
    this->type=WFileSystemTree::directory;
}

WFileSystemNode::WFileSystemNode(QFile *file,QObject *parent) :
    QObject(parent),
    file(file){
    this->type=WFileSystemTree::file;
}

WFileSystemNode::WFileSystemNode(QDir *dir,QObject *parent) :
    QObject(parent),
    dir(dir){
    this->type=WFileSystemTree::directory;
}

WFileSystemNode::~WFileSystemNode(){
    foreach(QPointer<WFileSystemNode> node,this->children)
        node->deleteLater();
    if(this->type==WFileSystemTree::directory && this->dir!=0)
        delete this->dir;
    if(this->type==WFileSystemTree::file && this->file!=0)
        delete this->file;
}
