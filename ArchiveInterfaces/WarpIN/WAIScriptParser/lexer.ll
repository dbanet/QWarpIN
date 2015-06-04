%option c++ noyywrap nounput batch debug noinput

%{
#include "parser.h"
%}

 /********************/
 /*   LEXER STATES   */
 /********************/
%x IN_T_MSG
%x IN_T_TEXT
%x IN_T_TITLE
%x IN_T_COMMENT
%x IN_T_WARPIN_BEGIN
%x IN_T_GROUP_BEGIN
%x IN_T_README_BEGIN
%x IN_T_README
%x IN_T_NEXTBUTTON_BEGIN
%x IN_T_NEXTBUTTON
%x IN_T_PCK_BEGIN
%x IN_T_PCK
%x IN_T_PAGE_BEGIN
%x IN_T_REXX_BEGIN
%x IN_T_REXX
%x IN_T_VARPROMPT
%x IN_T_VARPROMPT_BEGIN
%x IN_T_VARPROMPT_BEGIN_TAT_NAME
%x IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE

 /****************************************/
 /****************************************/
 /****************************************/

%%

 /****************************/
 /*     BODY, HEAD TAGS      */
 /*   (have no attributes)   */
 /****************************/

(?i:<[ \t\r\n]*HEAD[ \t\r\n]*>)               return T_HEAD_BEGIN;
(?i:<[ \t\r\n]*\/[ \t\r\n]*HEAD[ \t\r\n]*>)   return T_HEAD_END;
(?i:<[ \t\r\n]*BODY[ \t\r\n]*>)               return T_BODY_BEGIN;
(?i:<[ \t\r\n]*\/[ \t\r\n]*BODY[ \t\r\n]*>)   return T_BODY_END;

 /*******************************/
 /*        WARPIN TAG           */
 /* (contains other tags in it) */
 /*******************************/

(?i:<[ \t\r\n]*WARPIN[ \t\r\n]*)            { yy_push_state(IN_T_WARPIN_BEGIN);
                                              return T_WARPIN_BEGIN_OPEN; }
(?i:<[ \t\r\n]*\/[ \t\r\n]*WARPIN[ \t\r\n]*>) return T_WARPIN_END;
<IN_T_WARPIN_BEGIN>{
	/**********************/
	/* <WARPIN> ATTRIBUTES */
	/**********************/
	(?i:VERSION)                          return TAT_VERSION;
	(?i:OS)                               return TAT_OS;
	(?i:CODEPAGE)                         return TAT_CODEPAGE;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	["][^"]*["]                           return T_STRING;
	['][^']*[']                           return T_STRING;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      return T_WARPN_BEGIN_CLOSE; }
}

 /*******************************/
 /*         GROUP TAG           */
 /* (contains other tags in it) */
 /*******************************/

(?i:<[ \t\r\n]*GROUP[ \t\r\n]*)             { yy_push_state(IN_T_GROUP_BEGIN);
                                              return T_GROUP_BEGIN_OPEN; }
(?i:<[ \t\r\n]*\/[ \t\r\n]*GROUP[ \t\r\n]*>)  return T_GROUP_END;
<IN_T_GROUP_BEGIN>{
	/**********************/
	/* <GROUP> ATTRIBUTES */
	/**********************/
	(?i:TITLE)                            return TAT_TITLE;
	(?i:EXPANDED)                         return TAT_EXPANDED;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	["][^"]*["]                           return T_STRING;
	['][^']*[']                           return T_STRING;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      return T_GROUP_BEGIN_CLOSE; }
}

 /********************************/
 /*        VARPROMPT TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

(?i:<[ \t\r\n]*VARPROMPT[ \t\r\n]*)         { yy_push_state(IN_T_VARPROMPT_BEGIN);
                                              return T_VARPROMPT_BEGIN_OPEN; }
<IN_T_VARPROMPT_BEGIN>{
	/**************************/
	/* <VARPROMPT> ATTRIBUTES */
	/**************************/
	(?i:NAME)                           { yy_push_state(IN_T_VARPROMPT_BEGIN_TAT_NAME);
                                              return TAT_NAME; }
	(?i:TYPE)                             return TAT_TYPE;
	"="                                   return T_EQUIV;
	(?i:["]?ALPHA["]?)                    return TATV_ALPHA;
	(?i:[']?ALPHA[']?)                    return TATV_ALPHA;
	(?i:["]?PATH["]?)                     return TATV_PATH;
	(?i:[']?PATH[']?)                     return TATV_PATH;
	(?i:["]?FAIL["]?)                     return TATV_FAIL;
	(?i:[']?FAIL[']?)                     return TATV_FAIL;
	(?i:["]?NUM\([0-9]+\,[0-9]+\)["]?)    return TATV_NUMERIC_RANGE;
	(?i:[']?NUM\([0-9]+\,[0-9]+\)[']?)    return TATV_NUMERIC_RANGE;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      yy_push_state(IN_T_VARPROMPT);
	                                      return T_VARPROMPT_BEGIN_CLOSE; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME>{
	"="                                 { yy_pop_state();
	                                      yy_push_state(IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE);
	                                      return T_EQUIV; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE>{
	["][^"]*["]                         { yy_pop_state();
	                                      return T_STRING; }
	['][^']*[']                         { yy_pop_state();
	                                      return T_STRING; }
	[^ ]+                               { yy_pop_state();
	                                      return T_STRING; }
}

<IN_T_VARPROMPT>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*VARPROMPT[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_VARPROMPT_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /*******************************/
 /*         PAGE TAG            */
 /* (contains other tags in it) */
 /*******************************/

(?i:<[ \t\r\n]*PAGE[ \t\r\n]*)              { yy_push_state(IN_T_PAGE_BEGIN);
                                              return T_PAGE_BEGIN_OPEN; }
(?i:<[ \t\r\n]*\/[ \t\r\n]*PAGE[ \t\r\n]*>)   return T_PAGE_END;
<IN_T_PAGE_BEGIN>{
	/*********************/
	/* <PAGE> ATTRIBUTES */
	/*********************/
	(?i:INDEX)                            return TAT_INDEX;
	(?i:TYPE)                             return TAT_TYPE;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	(?i:["]?TEXT["]?)                     return TATV_TEXT;
	(?i:[']?TEXT[']?)                     return TATV_TEXT;
	(?i:["]?README["]?)                   return TATV_README;
	(?i:[']?README[']?)                   return TATV_README;
	(?i:["]?CONTAINER["]?)                return TATV_CONTAINER;
	(?i:[']?CONTAINER[']?)                return TATV_CONTAINER;
	(?i:["]?CONFIGURE["]?)                return TATV_CONFIGURE;
	(?i:[']?CONFIGURE[']?)                return TATV_CONFIGURE;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      return T_PAGE_BEGIN_CLOSE; }
}

 /*******************************/
 /*           MSG TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

(?i:<[ \t\r\n]*MSG[ \t\r\n]*>)              { yy_push_state(IN_T_MSG);
                                              return T_MSG_BEGIN; }
<IN_T_MSG>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*MSG[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_MSG_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /*******************************/
 /*          TEXT TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

(?i:<[ \t\r\n]*TEXT[ \t\r\n]*>)             { yy_push_state(IN_T_TEXT);
                                              return T_TEXT_BEGIN; }
<IN_T_TEXT>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*TEXT[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_TEXT_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /*******************************/
 /*         TITLE TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

(?i:<[ \t\r\n]*TITLE[ \t\r\n]*>)            { yy_push_state(IN_T_TITLE);
                                              return T_TITLE_BEGIN; }
<IN_T_TITLE>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*TITLE[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_TITLE_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /********************************/
 /*       NEXTBUTTON TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

(?i:<[ \t\r\n]*NEXTBUTTON[ \t\r\n]*)        { yy_push_state(IN_T_NEXTBUTTON_BEGIN);
                                              return T_NEXTBUTTON_BEGIN_OPEN; }
<IN_T_NEXTBUTTON_BEGIN>{
	/***************************/
	/* <NEXTBUTTON> ATTRIBUTES */
	/***************************/
	(?i:TARGET)                           return TAT_TARGET;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      yy_push_state(IN_T_NEXTBUTTON);
	                                      return T_NEXTBUTTON_BEGIN_CLOSE; }
}

<IN_T_NEXTBUTTON>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*NEXTBUTTON[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_NEXTBUTTON_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /********************************/
 /*        README TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

(?i:<[ \t\r\n]*README[ \t\r\n]*)            { yy_push_state(IN_T_README_BEGIN);
                                              return T_README_BEGIN_OPEN; }
<IN_T_README_BEGIN>{
	/***********************/
	/* <README> ATTRIBUTES */
	/***********************/
	(?i:FORMAT)                           return TAT_FORMAT;
	(?i:EXTRACTFROMPCK)                   return TAT_FORMAT;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	(?i:["]?PLAIN["]?)                    return TATV_PLAIN;
	(?i:[']?PLAIN[']?)                    return TATV_PLAIN;
	(?i:["]?FLOW["]?)                     return TATV_FLOW;
	(?i:[']?FLOW[']?)                     return TATV_FLOW;
	(?i:["]?HTML["]?)                     return TATV_HTML;
	(?i:[']?HTML[']?)                     return TATV_HTML;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      yy_push_state(IN_T_README);
	                                      return T_README_BEGIN_CLOSE; }
}

<IN_T_README>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*README[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_README_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /********************************/
 /*          PCK TAG             */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

(?i:<[ \t\r\n]*PCK[ \t\r\n]*)               { yy_push_state(IN_T_PCK_BEGIN);
                                              return T_PCK_BEGIN_OPEN; }
<IN_T_PCK_BEGIN>{
	/********************/
	/* <PCK> ATTRIBUTES */
	/********************/
	(?i:INDEX)                            return TAT_INDEX;
	(?i:PACKAGEID)                        return TAT_PACKAGEID;
	(?i:TITLE)                            return TAT_TITLE;
	(?i:EXTERNAL)                         return TAT_EXTERNAL;
	(?i:TARGET)                           return TAT_TARGET;
	(?i:BASE)                             return TAT_BASE;
	(?i:NOBASE)                           return TAT_NOBASE;
	(?i:FIXED)                            return TAT_FIXED;
	(?i:SELECT)                           return TAT_SELECT;
	(?i:NODESELECT)                       return TAT_NODESELECT;
	(?i:LONGFILENAMES)                    return TAT_LONGFILENAMES;
	(?i:REQUIRES)                         return TAT_REQUIRES;
	(?i:CONFIGSYS)                        return TAT_CONFIGSYS;
	(?i:REGISTERCLASS)                    return TAT_REGISTERCLASS;
	(?i:REPLACECLASS)                     return TAT_REPLACECLASS;
	(?i:CREATEOBJECT)                     return TAT_CREATEOBJECT;
	(?i:EXECUTE)                          return TAT_EXECUTE;
	(?i:DEEXECUTE)                        return TAT_DEEXECUTE;
	(?i:CLEARPROFILE)                     return TAT_CLEARPROFILE;
	(?i:WRITEPROFILE)                     return TAT_WRITEPROFILE;
	(?i:KILLPROCESS)                      return TAT_KILLPROCESS;
	"="                                   return T_EQUIV;
	["]?[0-9]+["]?                        return T_NUMBER;
	[']?[0-9]+[']?                        return T_NUMBER;
	["][^"]*["]                           return T_STRING;
	['][^']*[']                           return T_STRING;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      yy_push_state(IN_T_PCK);
	                                      return T_PCK_BEGIN_CLOSE; }
}

<IN_T_PCK>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*PCK[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_PCK_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /********************************/
 /*          REXX TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

(?i:<[ \t\r\n]*REXX[ \t\r\n]*)              { yy_push_state(IN_T_REXX_BEGIN);
                                              return T_README_REXX_OPEN; }
<IN_T_REXX_BEGIN>{
	/*********************/
	/* <REXX> ATTRIBUTES */
	/*********************/
	(?i:NAME)                             return TAT_NAME;
	"="                                   return T_EQUIV;
	["][^"]*["]                           return T_STRING;
	['][^']*[']                           return T_STRING;
	([ \t\r\n]*)                          ;
	">"                                 { yy_pop_state();
	                                      yy_push_state(IN_T_REXX);
	                                      return T_REXX_BEGIN_CLOSE; }
}

<IN_T_REXX>{
	(?i:<[ \t\r\n]*\/[ \t\r\n]*REXX[ \t\r\n]*>) {
	                                      yy_pop_state();
	                                      return T_REXX_END; }
	"<"                                   return T_STRING;
	[^<]+?/<                              return T_STRING;
}

 /********************************/
 /*        COMMENT TAG           */
 /* (everything inside until the */
 /*  closing tag is ignored)     */
 /********************************/

"<!--"                                      { yy_push_state(IN_T_COMMENT);
                                              return T_COMMENT_BEGIN; }
<IN_T_COMMENT>{
	"-->"                               { yy_pop_state();
	                                      return T_COMMENT_END; }
	"-"                                   ;
	[^-]+?/-                              ;
}

[ \t\r\n]                                     ;
<*>.                                          std::terminate();

%%