#include "wfilesystem.h"

WFile::WFile(QString fileName,QObject *parent) :
    QFile(fileName,parent){
}

void WFile::setSize(qint64 fileSize){
    this->fileSize=fileSize;
}

qint64 WFile::size() const{
    return this->fileSize;
}

qint64 WFile::readData(char *data,qint64 length){
    return this->readDataFn(data,length,this);
}

void WFile::setReadDataFn(readDataCallbackFn fn){
    this->readDataFn=fn;
}
