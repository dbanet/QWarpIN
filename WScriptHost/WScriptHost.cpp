#include "WScriptHost.h"

WScriptHost::WScriptHost(QObject *parent) :
    QObject(parent)
{
    this->context=this->freestandingContext;
    qScriptRegisterMetaType(&this->engine,WScriptHost::WFileToScriptValue,WScriptHost::WFileFromScriptValue);
    qScriptRegisterMetaType(&this->engine,WScriptHost::WFileSystemTreeToScriptValue,WScriptHost::WFileSystemTreeFromScriptValue);
    qScriptRegisterMetaType(&this->engine,WScriptHost::WFileSystemNodeToScriptValue,WScriptHost::WFileSystemNodeFromScriptValue);
}

void WScriptHost::setInstallationContext(QString script, WFileSystemTree* files, QString systemEnvironment){
    this->context=this->installationContext;
    this->script=script;
    this->files=files;
    this->systemEnvironment=systemEnvironment;

    QScriptValue hostConstructor=this->engine.evaluate(loadModule(":/WSH/js/Host.js")); NGNEXC(this->engine);
    hostConstructor.property("_native").setProperty("log",this->engine.newFunction(log)); NGNEXC(this->engine);
    this->engine.globalObject().setProperty("$",hostConstructor.construct()); NGNEXC(this->engine);

    QScriptValue installationConstructor=engine.evaluate(this->script); NGNEXC(this->engine);
    this->installationObject=installationConstructor.construct(QScriptValueList()<<this->engine.newQObject(this->files)<<systemEnvironment); NGNEXC(this->engine);
    this->pkgInfo=this->installationObject.property("pkgInfo").call(this->installationObject); NGNEXC(this->engine);
    this->installationObject.property("atInit").call(this->installationObject); NGNEXC(this->engine);
}

WInstallationInformation WScriptHost::install(){
    if(this->context!=this->installationContext)
        throw new E_WSH_InvalidContext;

    this->systemEnvironment=this->installationObject.property("fillResponse").call(this->installationObject).toString(); NGNEXC(this->engine);
    QString reportScript=this->installationObject.property("install").call(this->installationObject).toString(); NGNEXC(this->engine);
    return WInstallationInformation(this->pkgInfo.property("packageID").toString(),reportScript);
}

QScriptValue WScriptHost::log(QScriptContext *context,QScriptEngine */*engine*/){
    if(context->argumentCount()==1)
        qDebug()<<qPrintable(context->argument(0).toString());
    return QScriptValue();
}

/* conversion functions */

QScriptValue WScriptHost::WFileToScriptValue(QScriptEngine *engine,WFile* const &in){
    return engine->newQObject(in);
}
void WScriptHost::WFileFromScriptValue(const QScriptValue &object,WFile* &out){
    out=qobject_cast<WFile*>(object.toQObject());
}
QScriptValue WScriptHost::WFileSystemTreeToScriptValue(QScriptEngine *engine,WFileSystemTree* const &in){
    return engine->newQObject(in);
}
void WScriptHost::WFileSystemTreeFromScriptValue(const QScriptValue &object,WFileSystemTree* &out){
    out=qobject_cast<WFileSystemTree*>(object.toQObject());
}
QScriptValue WScriptHost::WFileSystemNodeToScriptValue(QScriptEngine *engine,WFileSystemNode* const &in){
    return engine->newQObject(in);
}
void WScriptHost::WFileSystemNodeFromScriptValue(const QScriptValue &object,WFileSystemNode* &out){
    out=qobject_cast<WFileSystemNode*>(object.toQObject());
}
