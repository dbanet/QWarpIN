#include "wfilesystem.h"

WFileSystemNode::WFileSystemNode(QFile *file,QList<WFileSystemNode*> &children,QVariant userData,QObject *parent) :
    QObject(parent),
    file(file),
    userData(userData){
    foreach(WFileSystemNode *node,children)
        this->children.append(QPointer<WFileSystemNode>(node));
    this->type=WFileSystem::file;
}

WFileSystemNode::WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QVariant userData,QObject *parent) :
    QObject(parent),
    dir(dir),
    userData(userData){
    foreach(WFileSystemNode *node,children)
        this->children.append(QPointer<WFileSystemNode>(node));
    this->type=WFileSystem::directory;
}

WFileSystemNode::WFileSystemNode(QFile *file,QVariant userData,QObject *parent) :
    QObject(parent),
    file(file),
    userData(userData){
    this->type=WFileSystem::file;
}

WFileSystemNode::WFileSystemNode(QDir *dir,QVariant userData,QObject *parent) :
    QObject(parent),
    dir(dir),
    userData(userData){
    this->type=WFileSystem::directory;
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
    if(this->type==WFileSystem::directory)
        return this->dir->dirName();
    else
        return this->file->fileName();
}

QString WFileSystemNode::toJSON(){
    QString childrenJSON;
    foreach(WFileSystemNode *child,this->children)
        childrenJSON+=child->toJSON()+",";
    childrenJSON.remove(childrenJSON.length()-1,1);
    return "{\"nodeName\":\""+this->getNodeName()+"\",\"nodeType\":\""+(this->type==WFileSystem::directory?"directory":"file")+"\",\"nodeChildren\":["+childrenJSON+"]}";
}

WFileSystemNode::~WFileSystemNode(){
    foreach(QPointer<WFileSystemNode> node,this->children)
        node->deleteLater();
    if(this->type==WFileSystem::directory && this->dir!=0)
        delete this->dir;
    if(this->type==WFileSystem::file && this->file!=0)
        delete this->file;
}
