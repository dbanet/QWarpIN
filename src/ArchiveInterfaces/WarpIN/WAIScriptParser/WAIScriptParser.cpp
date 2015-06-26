#include "WAIScriptParser.h"
#include "Parser.h"
#include <sstream>

WAIScriptParser::WAIScriptParser(QString WarpINScript):
	wpScript(WarpINScript){
	this->wpScriptStream=new std::istringstream(this->wpScript.toStdString());
	this->parser=new Parser(*this->wpScriptStream,this);
}

QString WAIScriptParser::translate(){
	this->parser->parse();
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

WAIScriptParser::~WAIScriptParser(){
	delete this->parser;
	delete this->wpScriptStream;
}
