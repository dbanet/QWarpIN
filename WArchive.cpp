#include "WArchive.h"
#include "WFileSystem/WFileSystem.h"
#include "WScriptHost/WScriptHost.h"

WArchive::WArchive(QFile *archiveFile,QObject *parent) :
    QObject(parent){
    auto interfaces=getArchiveInterfaces();
    auto interfaceIDs=interfaces->list();

    if(!archiveFile->open(QIODevice::ReadOnly))
        throw E_WA_FileNotFound();
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
        catch(exception &e){
            qDebug(e.what());
            this->interface=0;
            ++it;
        }
        catch(...){
            this->interface=0;
            ++it;
        }
    if(interface.isNull())
        throw E_WA_CannotFindSuitableArchiveInterface();

    qDebug()<<"The archive is of type"<<this->interface->id();
    qDebug()<<"The archive's name is"<<this->name();
}
QString WArchive::name(){
    return this->interface->arcName();
}
QFile* WArchive::file(){
    return this->interface->arcFile();
}
void WArchive::install(QString systemEnvironment){
    try{
        WScriptHost host;
        host.setInstallationContext(this->interface->script(),this->interface->files(),systemEnvironment);
        WInstallationInformation info=host.install();
    }
    catch(exception &e){
        qDebug(e.what());
    }
}
void WArchive::test(){

}
