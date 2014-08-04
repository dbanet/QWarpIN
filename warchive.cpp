#include "warchive.h"
#include "filesystemtree.h"

WArchive::WArchive(QFile *archiveFile,QObject *parent) :
    QObject(parent){
    WArchiveInterfaceFactory *interfaces=getArchiveInterfaces();
    for(auto it=interfaces->list().begin();it!=interfaces->list().end();)
        try{
            this->interface=interfaces->get(*it)(archiveFile);
            break;
        }
        catch(E_WAI_InstantiationError){
            this->interface=0;
            ++it;
        }
        catch(...){
            this->interface=0;
            ++it;
        }
    if(interface.isNull())
        throw new E_WA_CannotFindSuitableArchiveInterface;

    qDebug()<<"The archive is of type"<<this->interface->id();
    qDebug()<<"The archive's name is"<<this->name();
}
QString WArchive::name(){
    return this->interface->arcName();
}
QFile* WArchive::file(){
    return this->interface->arcFile();
}
void WArchive::test(){

}
