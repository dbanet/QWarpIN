%case-insensitive

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

\<[ \t\r\n]*HEAD[ \t\r\n]*\>                     return Parser::T_HEAD_BEGIN;
\<[ \t\r\n]*\/[ \t\r\n]*HEAD[ \t\r\n]*\>         return Parser::T_HEAD_END;
\<[ \t\r\n]*BODY[ \t\r\n]*\>                     return Parser::T_BODY_BEGIN;
\<[ \t\r\n]*\/[ \t\r\n]*BODY[ \t\r\n]*\>         return Parser::T_BODY_END;

 /*******************************/
 /*        WARPIN TAG           */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*WARPIN[ \t\r\n]*                   { push_state(StartCondition__::IN_T_WARPIN_BEGIN);
                                                 return Parser::T_WARPIN_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*WARPIN[ \t\r\n]*\>       return Parser::T_WARPIN_END;
<IN_T_WARPIN_BEGIN>{
	/**********************/
	/* <WARPIN> ATTRIBUTES */
	/**********************/
	VERSION                                  return Parser::TAT_VERSION;
	OS                                       return Parser::TAT_OS;
	CODEPAGE                                 return Parser::TAT_CODEPAGE;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	["][^"]*["]                              return Parser::T_STRING;
	['][^']*[']                              return Parser::T_STRING;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         return Parser::T_WARPN_BEGIN_CLOSE; }
}

 /*******************************/
 /*         GROUP TAG           */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*GROUP[ \t\r\n]*                    { push_state(StartCondition__::IN_T_GROUP_BEGIN);
                                                 return Parser::T_GROUP_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*GROUP[ \t\r\n]*\>        return Parser::T_GROUP_END;
<IN_T_GROUP_BEGIN>{
	/**********************/
	/* <GROUP> ATTRIBUTES */
	/**********************/
	TITLE                                    return Parser::TAT_TITLE;
	EXPANDED                                 return Parser::TAT_EXPANDED;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	["][^"]*["]                              return Parser::T_STRING;
	['][^']*[']                              return Parser::T_STRING;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         return Parser::T_GROUP_BEGIN_CLOSE; }
}

 /********************************/
 /*        VARPROMPT TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*VARPROMPT[ \t\r\n]*                { push_state(StartCondition__::IN_T_VARPROMPT_BEGIN);
                                                 return Parser::T_VARPROMPT_BEGIN_OPEN; }
<IN_T_VARPROMPT_BEGIN>{
	/**************************/
	/* <VARPROMPT> ATTRIBUTES */
	/**************************/
	NAME                                   { push_state(StartCondition__::IN_T_VARPROMPT_BEGIN_TAT_NAME);
                                                 return Parser::TAT_NAME; }
	TYPE                                     return Parser::TAT_TYPE;
	"="                                      return Parser::T_EQUIV;
	["]?ALPHA["]?                            return Parser::TATV_ALPHA;
	[']?ALPHA[']?                            return Parser::TATV_ALPHA;
	["]?PATH["]?                             return Parser::TATV_PATH;
	[']?PATH[']?                             return Parser::TATV_PATH;
	["]?FAIL["]?                             return Parser::TATV_FAIL;
	[']?FAIL[']?                             return Parser::TATV_FAIL;
	["]?NUM\([0-9]+\,[0-9]+\)["]?            return Parser::TATV_NUMERIC_RANGE;
	[']?NUM\([0-9]+\,[0-9]+\)[']?            return Parser::TATV_NUMERIC_RANGE;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_VARPROMPT);
	                                         return Parser::T_VARPROMPT_BEGIN_CLOSE; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME>{
	"="                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE);
	                                         return Parser::T_EQUIV; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE>{
	["][^"]*["]                            { pop_state();
	                                         return Parser::T_STRING; }
	['][^']*[']                            { pop_state();
	                                         return Parser::T_STRING; }
	[^ ]+                                  { pop_state();
	                                         return Parser::T_STRING; }
}

<IN_T_VARPROMPT>{
	\<[ \t\r\n]*\/[ \t\r\n]*VARPROMPT[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_VARPROMPT_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /*******************************/
 /*         PAGE TAG            */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*PAGE[ \t\r\n]*                     { push_state(StartCondition__::IN_T_PAGE_BEGIN);
                                                 return Parser::T_PAGE_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*PAGE[ \t\r\n]*\>         return Parser::T_PAGE_END;
<IN_T_PAGE_BEGIN>{
	/*********************/
	/* <PAGE> ATTRIBUTES */
	/*********************/
	INDEX                                    return Parser::TAT_INDEX;
	TYPE                                     return Parser::TAT_TYPE;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	["]?TEXT["]?                             return Parser::TATV_TEXT;
	[']?TEXT[']?                             return Parser::TATV_TEXT;
	["]?README["]?                           return Parser::TATV_README;
	[']?README[']?                           return Parser::TATV_README;
	["]?CONTAINER["]?                        return Parser::TATV_CONTAINER;
	[']?CONTAINER[']?                        return Parser::TATV_CONTAINER;
	["]?CONFIGURE["]?                        return Parser::TATV_CONFIGURE;
	[']?CONFIGURE[']?                        return Parser::TATV_CONFIGURE;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         return Parser::T_PAGE_BEGIN_CLOSE; }
}

 /*******************************/
 /*           MSG TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*MSG[ \t\r\n]*\>                    { push_state(StartCondition__::IN_T_MSG);
                                                 return Parser::T_MSG_BEGIN; }
<IN_T_MSG>{
	\<[ \t\r\n]*\/[ \t\r\n]*MSG[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_MSG_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /*******************************/
 /*          TEXT TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*TEXT[ \t\r\n]*\>                   { push_state(StartCondition__::IN_T_TEXT);
                                                 return Parser::T_TEXT_BEGIN; }
<IN_T_TEXT>{
	\<[ \t\r\n]*\/[ \t\r\n]*TEXT[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_TEXT_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /*******************************/
 /*         TITLE TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*TITLE[ \t\r\n]*\>                  { push_state(StartCondition__::IN_T_TITLE);
                                                 return Parser::T_TITLE_BEGIN; }
<IN_T_TITLE>{
	\<[ \t\r\n]*\/[ \t\r\n]*TITLE[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_TITLE_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /********************************/
 /*       NEXTBUTTON TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*NEXTBUTTON[ \t\r\n]*               { push_state(StartCondition__::IN_T_NEXTBUTTON_BEGIN);
                                                 return Parser::T_NEXTBUTTON_BEGIN_OPEN; }
<IN_T_NEXTBUTTON_BEGIN>{
	/***************************/
	/* <NEXTBUTTON> ATTRIBUTES */
	/***************************/
	TARGET                                   return Parser::TAT_TARGET;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_NEXTBUTTON);
	                                         return Parser::T_NEXTBUTTON_BEGIN_CLOSE; }
}

<IN_T_NEXTBUTTON>{
	\<[ \t\r\n]*\/[ \t\r\n]*NEXTBUTTON[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_NEXTBUTTON_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /********************************/
 /*        README TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*README[ \t\r\n]*                   { push_state(StartCondition__::IN_T_README_BEGIN);
                                                 return Parser::T_README_BEGIN_OPEN; }
<IN_T_README_BEGIN>{
	/***********************/
	/* <README> ATTRIBUTES */
	/***********************/
	FORMAT                                   return Parser::TAT_FORMAT;
	EXTRACTFROMPCK                           return Parser::TAT_FORMAT;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	["]?PLAIN["]?                            return Parser::TATV_PLAIN;
	[']?PLAIN[']?                            return Parser::TATV_PLAIN;
	["]?FLOW["]?                             return Parser::TATV_FLOW;
	[']?FLOW[']?                             return Parser::TATV_FLOW;
	["]?HTML["]?                             return Parser::TATV_HTML;
	[']?HTML[']?                             return Parser::TATV_HTML;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_README);
	                                         return Parser::T_README_BEGIN_CLOSE; }
}

<IN_T_README>{
	\<[ \t\r\n]*\/[ \t\r\n]*README[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_README_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /********************************/
 /*          PCK TAG             */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*PCK[ \t\r\n]*                      { push_state(StartCondition__::IN_T_PCK_BEGIN);
                                                 return Parser::T_PCK_BEGIN_OPEN; }
<IN_T_PCK_BEGIN>{
	/********************/
	/* <PCK> ATTRIBUTES */
	/********************/
	INDEX                                    return Parser::TAT_INDEX;
	PACKAGEID                                return Parser::TAT_PACKAGEID;
	TITLE                                    return Parser::TAT_TITLE;
	EXTERNAL                                 return Parser::TAT_EXTERNAL;
	TARGET                                   return Parser::TAT_TARGET;
	BASE                                     return Parser::TAT_BASE;
	NOBASE                                   return Parser::TAT_NOBASE;
	FIXED                                    return Parser::TAT_FIXED;
	SELECT                                   return Parser::TAT_SELECT;
	NODESELECT                               return Parser::TAT_NODESELECT;
	LONGFILENAMES                            return Parser::TAT_LONGFILENAMES;
	REQUIRES                                 return Parser::TAT_REQUIRES;
	CONFIGSYS                                return Parser::TAT_CONFIGSYS;
	REGISTERCLASS                            return Parser::TAT_REGISTERCLASS;
	REPLACECLASS                             return Parser::TAT_REPLACECLASS;
	CREATEOBJECT                             return Parser::TAT_CREATEOBJECT;
	EXECUTE                                  return Parser::TAT_EXECUTE;
	DEEXECUTE                                return Parser::TAT_DEEXECUTE;
	CLEARPROFILE                             return Parser::TAT_CLEARPROFILE;
	WRITEPROFILE                             return Parser::TAT_WRITEPROFILE;
	KILLPROCESS                              return Parser::TAT_KILLPROCESS;
	"="                                      return Parser::T_EQUIV;
	["]?[0-9]+["]?                           return Parser::T_NUMBER;
	[']?[0-9]+[']?                           return Parser::T_NUMBER;
	["][^"]*["]                              return Parser::T_STRING;
	['][^']*[']                              return Parser::T_STRING;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_PCK);
	                                         return Parser::T_PCK_BEGIN_CLOSE; }
}

<IN_T_PCK>{
	\<[ \t\r\n]*\/[ \t\r\n]*PCK[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_PCK_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /********************************/
 /*          REXX TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*REXX[ \t\r\n]*                     { push_state(StartCondition__::IN_T_REXX_BEGIN);
                                                 return Parser::T_README_REXX_OPEN; }
<IN_T_REXX_BEGIN>{
	/*********************/
	/* <REXX> ATTRIBUTES */
	/*********************/
	NAME                                     return Parser::TAT_NAME;
	"="                                      return Parser::T_EQUIV;
	["][^"]*["]                              return Parser::T_STRING;
	['][^']*[']                              return Parser::T_STRING;
	[ \t\r\n]                                ;
	">"                                    { pop_state();
	                                         push_state(StartCondition__::IN_T_REXX);
	                                         return Parser::T_REXX_BEGIN_CLOSE; }
}

<IN_T_REXX>{
	\<[ \t\r\n]*\/[ \t\r\n]*REXX[ \t\r\n]*\> {
	                                         pop_state();
	                                         return Parser::T_REXX_END; }
	"<"                                      return Parser::T_STRING;
	[^<]+?/\<                                return Parser::T_STRING;
}

 /********************************/
 /*        COMMENT TAG           */
 /* (everything inside until the */
 /*  closing tag is ignored)     */
 /********************************/

"<!--"                                         { push_state(StartCondition__::IN_T_COMMENT);
                                                 return Parser::T_COMMENT_BEGIN; }
<IN_T_COMMENT>{
	"-->"                                  { pop_state();
	                                         return Parser::T_COMMENT_END; }
	"-"                                      ;
	[^-]+?/-                                 ;
}

[ \t\r\n]                                        ;
<*>.                                             std::terminate();

%%