#include "WFileSystem.h"

WFileSystemNode::WFileSystemNode(WFile *file,QList<WFileSystemNode*> &children,QObject *parent) :
	QObject(parent){
	foreach(WFileSystemNode *node,children)
		this->children.append(QPointer<WFileSystemNode>(node));
	this->setFile(file);
}

WFileSystemNode::WFileSystemNode(QDir *dir,QList<WFileSystemNode*> &children,QObject *parent) :
	QObject(parent){
	foreach(WFileSystemNode *node,children)
		this->children.append(QPointer<WFileSystemNode>(node));
	this->setDir(dir);
}

WFileSystemNode::WFileSystemNode(WFile *file,QObject *parent) :
	QObject(parent){
	this->setFile(file);
}

WFileSystemNode::WFileSystemNode(QDir *dir,QObject *parent) :
	QObject(parent){
	this->setDir(dir);
}

WFile* WFileSystemNode::file(){
	return this->_file;
}

QDir* WFileSystemNode::dir(){
	return this->_dir;
}

void WFileSystemNode::setFile(WFile *file){
	this->type=WFileSystem::file;
	this->_file=file;
	this->_file->setFSNode(this);
}

void WFileSystemNode::setDir(QDir *dir){
	this->type=WFileSystem::directory;
	this->_dir=dir;
}

/*!
 * Expropriates the given node assigning it as a child, taking care of possible
 * duplicates, where a child with the same name may already exist.
 * The given node (at least, by the same address) cannot be henceforth used by
 * the caller (think of it as the exclusive right to own the given node moves to
 * this node): if we allow this, then in case of a duplicate, when all the children
 * of a duplicated node/subnode of the given node get captured as children by this
 * node's child node with the same name, as of the duplicated node, the duplicated
 * node/subnode of the given node could have never been addressed by the caller anyway,
 * as the caller doesn't know which subnode of the given node, or the node itself has
 * caused duplication, and even if the duplication has taken place at all, so the
 * duplicated node should be deleted within this function to prevent memory leak.
 * This routine takes care of the memory and replaces the given pointer with another one,
 * addressed as this node's child, after the given node has been added as a child (but
 * it may, of course, happen to be equal, if no duplication has been taken place).
 * THEREFORE THE GIVEN ADDRESS OF THE NODE BECOMES INVALID: DO NOT USE ANY COPIES OF
 * THE GIVEN POINTER THAT HAVE BEEN DONE BEFORE THE FUNCTION CALL, OTHERWISE YOU RISK
 * TO CAUSE AN ACCESS VIOLATION BY READING UNALLOCATED MEMORY
 */
void WFileSystemNode::addChild(WFileSystemNode *node){
	qint64 equinamedChildId=-1; // have none
	for(int i=0;i<this->children.length();++i)
		if(this->children[i]->nodeName()==node->nodeName()){
			equinamedChildId=i;
			break;
		}

	if(!~equinamedChildId)
		this->children<<QPointer<WFileSystemNode>(node);
	else{
		auto childrenNodes=node->children;
		node->children.clear(); // if we don't detach the given node's children from it, then its
		// destructor will subsequently delete all of them upon the given node's deletion, i.e. now
		node->deleteLater();
		foreach(auto childNode,childrenNodes)
			this->children[equinamedChildId]->addChild(childNode);
		node=this->children[equinamedChildId];
	}
}

QString WFileSystemNode::nodeName(){
	if(this->type==WFileSystem::directory)
		return this->_dir->dirName();
	else
		return this->_file->fileName();
}

WFileSystemNode* WFileSystemNode::navigate(QString path){
	QStringList nodes=path.split('/');
	foreach(WFileSystemNode *child,this->children)
		if(child->nodeName()==nodes[0])
			return nodes.length()>1?(
				nodes.pop_front(),child->navigate(nodes.join("/"))
			):(
				child
			);
	throw E_WFS_FileNotFound();
}

QString WFileSystemNode::toJSON(){
	QString childrenJSON;
	foreach(WFileSystemNode *child,this->children)
		childrenJSON+=child->toJSON()+",";
	childrenJSON.remove(childrenJSON.length()-1,1);
	return "{\"nodeName\":\""+this->nodeName()+"\",\"nodeType\":\""+(this->type==WFileSystem::directory?"directory":"file")+"\",\"nodeChildren\":["+childrenJSON+"]}";
}

WFileSystemNode::~WFileSystemNode(){
	foreach(QPointer<WFileSystemNode> node,this->children)
		node->deleteLater();
	if(this->type==WFileSystem::directory && this->_dir!=0)
		delete this->_dir;
	if(this->type==WFileSystem::file && this->_file!=0)
		delete this->_file;
}
