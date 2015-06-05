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

\<[ \t\r\n]*HEAD[ \t\r\n]*\>                     return T_HEAD_BEGIN;
\<[ \t\r\n]*\/[ \t\r\n]HEAD[ \t\r\n]*\>          return T_HEAD_END;
\<[ \t\r\n]*BODY[ \t\r\n]*\>                     return T_BODY_BEGIN;
\<[ \t\r\n]*\/[ \t\r\n]*BODY[ \t\r\n]*\>         return T_BODY_END;

 /*******************************/
 /*        WARPIN TAG           */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*WARPIN[ \t\r\n]*                   { yy_push_state(IN_T_WARPIN_BEGIN);
                                                 return T_WARPIN_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*WARPIN[ \t\r\n]*\>       return T_WARPIN_END;
<IN_T_WARPIN_BEGIN>{
	/**********************/
	/* <WARPIN> ATTRIBUTES */
	/**********************/
	VERSION                                  return TAT_VERSION;
	OS                                       return TAT_OS;
	CODEPAGE                                 return TAT_CODEPAGE;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	["][^"]*["]                              return T_STRING;
	['][^']*[']                              return T_STRING;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         return T_WARPN_BEGIN_CLOSE; }
}

 /*******************************/
 /*         GROUP TAG           */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*GROUP[ \t\r\n]*                    { yy_push_state(IN_T_GROUP_BEGIN);
                                                 return T_GROUP_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*GROUP[ \t\r\n]*\>        return T_GROUP_END;
<IN_T_GROUP_BEGIN>{
	/**********************/
	/* <GROUP> ATTRIBUTES */
	/**********************/
	TITLE                                    return TAT_TITLE;
	EXPANDED                                 return TAT_EXPANDED;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	["][^"]*["]                              return T_STRING;
	['][^']*[']                              return T_STRING;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         return T_GROUP_BEGIN_CLOSE; }
}

 /********************************/
 /*        VARPROMPT TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*VARPROMPT[ \t\r\n]*                { yy_push_state(IN_T_VARPROMPT_BEGIN);
                                                 return T_VARPROMPT_BEGIN_OPEN; }
<IN_T_VARPROMPT_BEGIN>{
	/**************************/
	/* <VARPROMPT> ATTRIBUTES */
	/**************************/
	NAME                                   { yy_push_state(IN_T_VARPROMPT_BEGIN_TAT_NAME);
                                                 return TAT_NAME; }
	TYPE                                     return TAT_TYPE;
	"="                                      return T_EQUIV;
	["]?ALPHA["]?                            return TATV_ALPHA;
	[']?ALPHA[']?                            return TATV_ALPHA;
	["]?PATH["]?                             return TATV_PATH;
	[']?PATH[']?                             return TATV_PATH;
	["]?FAIL["]?                             return TATV_FAIL;
	[']?FAIL[']?                             return TATV_FAIL;
	["]?NUM\([0-9]+\,[0-9]+\)["]?            return TATV_NUMERIC_RANGE;
	[']?NUM\([0-9]+\,[0-9]+\)[']?            return TATV_NUMERIC_RANGE;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         yy_push_state(IN_T_VARPROMPT);
	                                         return T_VARPROMPT_BEGIN_CLOSE; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME>{
	"="                                    { yy_pop_state();
	                                         yy_push_state(IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE);
	                                         return T_EQUIV; }
}

<IN_T_VARPROMPT_BEGIN_TAT_NAME_VALUE>{
	["][^"]*["]                            { yy_pop_state();
	                                         return T_STRING; }
	['][^']*[']                            { yy_pop_state();
	                                         return T_STRING; }
	[^ ]+                                  { yy_pop_state();
	                                         return T_STRING; }
}

<IN_T_VARPROMPT>{
	\<[ \t\r\n]*\/[ \t\r\n]*VARPROMPT[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_VARPROMPT_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /*******************************/
 /*         PAGE TAG            */
 /* (contains other tags in it) */
 /*******************************/

\<[ \t\r\n]*PAGE[ \t\r\n]*                     { yy_push_state(IN_T_PAGE_BEGIN);
                                                 return T_PAGE_BEGIN_OPEN; }
\<[ \t\r\n]*\/[ \t\r\n]*PAGE[ \t\r\n]*\>         return T_PAGE_END;
<IN_T_PAGE_BEGIN>{
	/*********************/
	/* <PAGE> ATTRIBUTES */
	/*********************/
	INDEX                                    return TAT_INDEX;
	TYPE                                     return TAT_TYPE;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	["]?TEXT["]?                             return TATV_TEXT;
	[']?TEXT[']?                             return TATV_TEXT;
	["]?README["]?                           return TATV_README;
	[']?README[']?                           return TATV_README;
	["]?CONTAINER["]?                        return TATV_CONTAINER;
	[']?CONTAINER[']?                        return TATV_CONTAINER;
	["]?CONFIGURE["]?                        return TATV_CONFIGURE;
	[']?CONFIGURE[']?                        return TATV_CONFIGURE;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         return T_PAGE_BEGIN_CLOSE; }
}

 /*******************************/
 /*           MSG TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*MSG[ \t\r\n]*\>                    { yy_push_state(IN_T_MSG);
                                                 return T_MSG_BEGIN; }
<IN_T_MSG>{
	\<[ \t\r\n]*\/[ \t\r\n]*MSG[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_MSG_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /*******************************/
 /*          TEXT TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*TEXT[ \t\r\n]*\>                   { yy_push_state(IN_T_TEXT);
                                                 return T_TEXT_BEGIN; }
<IN_T_TEXT>{
	\<[ \t\r\n]*\/[ \t\r\n]*TEXT[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_TEXT_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /*******************************/
 /*         TITLE TAG           */
 /* (no attributes; everything  */
 /*  inside until the closing   */
 /*          tag is a T_STRING) */
 /*******************************/

\<[ \t\r\n]*TITLE[ \t\r\n]*\>                  { yy_push_state(IN_T_TITLE);
                                                 return T_TITLE_BEGIN; }
<IN_T_TITLE>{
	\<[ \t\r\n]*\/[ \t\r\n]*TITLE[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_TITLE_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /********************************/
 /*       NEXTBUTTON TAG         */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*NEXTBUTTON[ \t\r\n]*               { yy_push_state(IN_T_NEXTBUTTON_BEGIN);
                                                 return T_NEXTBUTTON_BEGIN_OPEN; }
<IN_T_NEXTBUTTON_BEGIN>{
	/***************************/
	/* <NEXTBUTTON> ATTRIBUTES */
	/***************************/
	TARGET                                   return TAT_TARGET;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         yy_push_state(IN_T_NEXTBUTTON);
	                                         return T_NEXTBUTTON_BEGIN_CLOSE; }
}

<IN_T_NEXTBUTTON>{
	\<[ \t\r\n]*\/[ \t\r\n]*NEXTBUTTON[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_NEXTBUTTON_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /********************************/
 /*        README TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*README[ \t\r\n]*                   { yy_push_state(IN_T_README_BEGIN);
                                                 return T_README_BEGIN_OPEN; }
<IN_T_README_BEGIN>{
	/***********************/
	/* <README> ATTRIBUTES */
	/***********************/
	FORMAT                                   return TAT_FORMAT;
	EXTRACTFROMPCK                           return TAT_FORMAT;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	["]?PLAIN["]?                            return TATV_PLAIN;
	[']?PLAIN[']?                            return TATV_PLAIN;
	["]?FLOW["]?                             return TATV_FLOW;
	[']?FLOW[']?                             return TATV_FLOW;
	["]?HTML["]?                             return TATV_HTML;
	[']?HTML[']?                             return TATV_HTML;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         yy_push_state(IN_T_README);
	                                         return T_README_BEGIN_CLOSE; }
}

<IN_T_README>{
	\<[ \t\r\n]*\/[ \t\r\n]*README[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_README_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /********************************/
 /*          PCK TAG             */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*PCK[ \t\r\n]*                      { yy_push_state(IN_T_PCK_BEGIN);
                                                 return T_PCK_BEGIN_OPEN; }
<IN_T_PCK_BEGIN>{
	/********************/
	/* <PCK> ATTRIBUTES */
	/********************/
	INDEX                                    return TAT_INDEX;
	PACKAGEID                                return TAT_PACKAGEID;
	TITLE                                    return TAT_TITLE;
	EXTERNAL                                 return TAT_EXTERNAL;
	TARGET                                   return TAT_TARGET;
	BASE                                     return TAT_BASE;
	NOBASE                                   return TAT_NOBASE;
	FIXED                                    return TAT_FIXED;
	SELECT                                   return TAT_SELECT;
	NODESELECT                               return TAT_NODESELECT;
	LONGFILENAMES                            return TAT_LONGFILENAMES;
	REQUIRES                                 return TAT_REQUIRES;
	CONFIGSYS                                return TAT_CONFIGSYS;
	REGISTERCLASS                            return TAT_REGISTERCLASS;
	REPLACECLASS                             return TAT_REPLACECLASS;
	CREATEOBJECT                             return TAT_CREATEOBJECT;
	EXECUTE                                  return TAT_EXECUTE;
	DEEXECUTE                                return TAT_DEEXECUTE;
	CLEARPROFILE                             return TAT_CLEARPROFILE;
	WRITEPROFILE                             return TAT_WRITEPROFILE;
	KILLPROCESS                              return TAT_KILLPROCESS;
	"="                                      return T_EQUIV;
	["]?[0-9]+["]?                           return T_NUMBER;
	[']?[0-9]+[']?                           return T_NUMBER;
	["][^"]*["]                              return T_STRING;
	['][^']*[']                              return T_STRING;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         yy_push_state(IN_T_PCK);
	                                         return T_PCK_BEGIN_CLOSE; }
}

<IN_T_PCK>{
	\<[ \t\r\n]*\/[ \t\r\n]*PCK[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_PCK_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /********************************/
 /*          REXX TAG            */
 /* (everything inside until the */
 /*  closing tag is a T_STRING)  */
 /********************************/

\<[ \t\r\n]*REXX[ \t\r\n]*                     { yy_push_state(IN_T_REXX_BEGIN);
                                                 return T_README_REXX_OPEN; }
<IN_T_REXX_BEGIN>{
	/*********************/
	/* <REXX> ATTRIBUTES */
	/*********************/
	NAME                                     return TAT_NAME;
	"="                                      return T_EQUIV;
	["][^"]*["]                              return T_STRING;
	['][^']*[']                              return T_STRING;
	[ \t\r\n]                                ;
	">"                                    { yy_pop_state();
	                                         yy_push_state(IN_T_REXX);
	                                         return T_REXX_BEGIN_CLOSE; }
}

<IN_T_REXX>{
	\<[ \t\r\n]*\/[ \t\r\n]*REXX[ \t\r\n]*\> {
	                                         yy_pop_state();
	                                         return T_REXX_END; }
	"<"                                      return T_STRING;
	[^<]+?/\<                                return T_STRING;
}

 /********************************/
 /*        COMMENT TAG           */
 /* (everything inside until the */
 /*  closing tag is ignored)     */
 /********************************/

"<!--"                                         { yy_push_state(IN_T_COMMENT);
                                                 return T_COMMENT_BEGIN; }
<IN_T_COMMENT>{
	"-->"                                  { yy_pop_state();
	                                         return T_COMMENT_END; }
	"-"                                      ;
	[^-]+?/-                                 ;
}

[ \t\r\n]                                        ;
<*>.                                             std::terminate();

%%