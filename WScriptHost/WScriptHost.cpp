#include "WScriptHost.h"

WScriptHost::WScriptHost(QObject *parent) :
    QObject(parent)
{
    QScriptValue hostConstructor=engine.evaluate(loadModule(":/WSH/js/Host.js"));
    hostConstructor.property("_native").setProperty("log",engine.newFunction(log));
    engine.globalObject().setProperty("$",hostConstructor.construct());
    engine.evaluate("$.log('yeaaaah!!!');");
}

QScriptValue WScriptHost::log(QScriptContext *context,QScriptEngine */*engine*/){
    if(context->argumentCount()==1)
        qDebug()<<qPrintable(context->argument(0).toString());
    return QScriptValue();
}
