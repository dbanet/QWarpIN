#include "WAIScriptParser.h"

WAIScriptParser::WAIScriptParser(QString WarpINScript):
    wpScript(WarpINScript)
{}

QString WAIScriptParser::translate(){
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
                    $.log(this.fs.navigate("/Pck2001/COPYING").file().open().readAll());
                }
            }
            return Package;
        })()
    )HEREDOC");
}
