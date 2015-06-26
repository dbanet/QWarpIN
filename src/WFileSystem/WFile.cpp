#include "WFileSystem.h"

WFile::WFile(QString fileName,QObject *parent) :
	QFile(fileName,parent),
	mode(NotOpen){
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
	if(this->isOpen())
		if(!cached)
			return this->readData(data,maxlen);
		else
			return this->readCachedData(data,maxlen);
	else
		return -1;
}

QByteArray WFile::read(qint64 maxlen){
	if(this->isOpen()){
		qint64 len;
		char *buf=new char[maxlen];
		if(!cached)
			len=this->readData(buf,maxlen);
		else
			len=this->readCachedData(buf,maxlen);
		QByteArray data(buf,len);
		delete[] buf;
		return data;
	}
	else
		return QByteArray();
}

QByteArray WFile::readAll(){
	return this->read(this->size());
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

WFile* WFile::open(){
	return this->openRO();
}

WFile* WFile::openRO(){
	mode=QIODevice::ReadOnly;
	return this;
}

WFile* WFile::openRW(){
	return this;
}

WFile::OpenMode WFile::openMode() const{
	return this->mode;
}

bool WFile::isOpen() const{
	return openMode()!=QIODevice::NotOpen;
}

void WFile::forceCache(){
	qint64 savedOffset=this->pos();
	this->cache=(char*)malloc(this->size());
	memset(this->cache,0,this->size());
	this->read(this->cache,this->size());
	this->cached=true;
	this->seek(savedOffset);
}

qint64 WFile::readCachedData(char *data, qint64 length){
	qint64 bytesToCopy=(length<this->size()-this->pos())?length:this->size()-this->pos();
	memcpy(data,this->cache+this->pos(),bytesToCopy);
	return bytesToCopy;
}

WFile::~WFile(){
	free(this->cache);
}
