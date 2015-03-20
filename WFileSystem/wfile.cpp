#include "wfilesystem.h"

WFile::WFile(QString fileName,QObject *parent) :
    mode(NotOpen),
    QFile(fileName,parent){
}

void WFile::setFSNode(WFileSystemNode *node){
    this->fsNode=node;
}

void WFile::setSize(qint64 fileSize){
    this->fileSize=fileSize;
}

qint64 WFile::size() const{
    return this->fileSize;
}

void WFile::setReadFn(readCallbackFn fn){
    this->readFn=fn;
}

qint64 WFile::readData(char *data,qint64 length){
    return this->readFn(data,length,this);
}

qint64 WFile::read(char *data, qint64 maxlen){
    return this->readData(data,maxlen);
}

void WFile::setSeekFn(seekCallbackFn fn){
    this->seekFn=fn;
}

bool WFile::seek(qint64 offset){
    return this->seekFn(offset,this);
}

void WFile::setPosFn(posCallbackFn fn){
    this->posFn=fn;
}

qint64 WFile::pos() const{
    return this->posFn(this);
}

void WFile::setCreationDateTime(QDateTime dateTime){
    this->fileCreationDateTime=dateTime;
}

QDateTime WFile::creationDateTime(){
    return this->fileCreationDateTime;
}

void WFile::setLastAccessDateTime(QDateTime dateTime){
    this->fileLastAccessDateTime=dateTime;
}

QDateTime WFile::lastAccessDateTime(){
    return this->fileLastAccessDateTime;
}

bool WFile::open(OpenMode mode){
    return mode==QIODevice::ReadOnly?this->mode=mode,true:false;
}

WFile::OpenMode WFile::openMode() const{
    return this->mode;
}
