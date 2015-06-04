#include "WAIScriptParser.h"
#include <QRegExp>

WAIScriptParser::WAIScriptParser(QString WarpINScript):
    wpScript(WarpINScript)
{}

QString WAIScriptParser::translate(){
    QString fixedWpScript=this->wpScript;
    fixedWpScript.replace(QRegExp("<HEAD>*</HEAD>",Qt::CaseInsensitive,QRegExp::Wildcard),"");
    qDebug()<<fixedWpScript;

    QDomNodeList list; QString errorMsg; int errorLine,errorColumn;
    if(!this->dom.setContent(fixedWpScript,false,&errorMsg,&errorLine,&errorColumn))
        throw E_WPIAI_ScriptParseError(QObject::tr("WarpINArchiveInterface has encountered an error parsing the script at line number %1, symbol number %2: %3").arg(QString::number(errorLine),QString::number(errorColumn),errorMsg));

    list=this->dom.elementsByTagName("WARPIN"); if(list.length()!=1)
        throw E_WPIAI_ScriptParseError(QObject::tr("The script contains %1 (should be 1) root <WARPIN> elements.").arg(list.length()));
    this->wp=list.item(0).toElement();

    list=this->wp.elementsByTagName("BODY"); if(list.length()!=1)
        throw E_WPIAI_ScriptParseError(QObject::tr("The script contains %1 (should be 1) <BODY> elements inside the root <WARPIN> element.").arg(list.length()));
    this->body=list.item(0).toElement();


    return QString(R"HEREDOC(
        (function(){
            var Package=function(fs,systemEnvironment){
                this.fs=fs;
                this.env=eval('('+systemEnvironment+')');
            }
            Package.prototype={
                pkgInfo:function(){
                    $.log("pkgInfo called!");
                    $.log(this.env.test);
                },
                atInit:function(){
                    $.log("atInit called!");
                },
                fillResponse:function(){
                    $.log("fillResponse called!");
                },
                install:function(){
                    $.log("install called!");
                    //$.log(this.fs.navigate("/Pck2001/COPYING").file().open().readAll());
                }
            }
            return Package;
        })()
    )HEREDOC");
}
