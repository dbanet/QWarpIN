#include "WArchive.h"
#include "WFileSystem/wfilesystem.h"

WArchive::WArchive(QFile *archiveFile,QObject *parent) :
    QObject(parent){
    auto interfaces=getArchiveInterfaces();
    auto interfaceIDs=interfaces->list();

    if(!archiveFile->open(QIODevice::ReadOnly))
        throw new E_WA_FileNotFound;
    else archiveFile->close();

    for(auto it=interfaceIDs.begin();it!=interfaceIDs.end();)
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
