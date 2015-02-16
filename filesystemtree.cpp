#include "filesystemtree.h"

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

void WFileSystemNode::addChild(WFileSystemNode *node){
    qint64 equinamedChildId=-1; // have none
    for(int i=0;i<this->children.length();++i)
        if(this->children[i]->getNodeName()==node->getNodeName()){
            equinamedChildId=i;
            break;
        }

    if(!~equinamedChildId)
        this->children<<QPointer<WFileSystemNode>(node);
    else
        foreach(WFileSystemNode *childNode,node->children)
            this->children[equinamedChildId]->addChild(childNode);
}

QString WFileSystemNode::getNodeName(){
    if(this->type==WFileSystemTree::directory)
        return this->dir->dirName();
    else
        return this->file->fileName();
}

QString WFileSystemNode::toJSON(){
    QString childrenJSON;
    foreach(WFileSystemNode *child,this->children)
        childrenJSON+=child->toJSON()+",";
    childrenJSON.remove(childrenJSON.length()-1,1);
    return "{\"nodeName\":\""+this->getNodeName()+"\",\"nodeType\":\""+(this->type==WFileSystemTree::directory?"directory":"file")+"\",\"nodeChildren\":["+childrenJSON+"]}";
}

WFileSystemNode::~WFileSystemNode(){
    foreach(QPointer<WFileSystemNode> node,this->children)
        node->deleteLater();
    if(this->type==WFileSystemTree::directory && this->dir!=0)
        delete this->dir;
    if(this->type==WFileSystemTree::file && this->file!=0)
        delete this->file;
}
