#include "WScriptHost.h"

WScriptHost::WScriptHost(QObject *parent) :
    QObject(parent)
{
    this->context=this->freestandingContext;
}

void WScriptHost::setInstallationContext(QString script, WFileSystemTree* files, QString systemEnvironment){
    this->context=this->installationContext;
    this->script=script;
    this->files=files;
    this->systemEnvironment=systemEnvironment;

    QScriptValue hostConstructor=engine.evaluate(loadModule(":/WSH/js/Host.js"));
    hostConstructor.property("_native").setProperty("log",engine.newFunction(log));
    engine.globalObject().setProperty("$",hostConstructor.construct());

    QScriptValue installationConstructor=engine.evaluate(this->script);
    this->installationObject=installationConstructor.construct(QScriptValueList()<<this->engine.newQObject(this->files)<<systemEnvironment);
    this->pkgInfo=this->installationObject.property("pkgInfo").call();
    this->installationObject.property("atInit").call();
}

WInstallationInformation WScriptHost::install(){
    if(this->context!=this->installationContext)
        throw new E_WSH_InvalidContext;

    this->systemEnvironment=this->installationObject.property("fillResponse").call().toString();
    QString reportScript=this->installationObject.property("install").call().toString();
    return WInstallationInformation(this->pkgInfo.property("packageID").toString(),reportScript);
}

QScriptValue WScriptHost::log(QScriptContext *context,QScriptEngine */*engine*/){
    if(context->argumentCount()==1)
        qDebug()<<qPrintable(context->argument(0).toString());
    return QScriptValue();
}
