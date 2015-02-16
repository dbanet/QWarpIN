#include "wfilesystem.h"

WFileSystemTree::WFileSystemTree(WFileSystemNode *rootNode,QObject *parent) :
    QObject(parent){
    this->root=rootNode;
}

WFileSystemNode* WFileSystemTree::rootNode(){
    return this->root;
}

void WFileSystemTree::setRootNode(WFileSystemNode *rootNode){
    this->root=rootNode;
}

void WFileSystemTree::addChild(WFileSystemNode *node){
    if(this->rootNode()==0)
        this->setRootNode(node);
    else
        this->rootNode()->addChild(node);
}

QString WFileSystemTree::toJSON(){
    if(this->rootNode()==0)
        return "{}";
    else
        return "{\"root\":"+this->rootNode()->toJSON()+"}";
}

WFileSystemTree::~WFileSystemTree(){
    delete this->root;
}
