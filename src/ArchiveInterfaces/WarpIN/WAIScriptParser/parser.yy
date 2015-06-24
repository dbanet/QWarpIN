%scanner Scanner.h
%token TATV_ALPHA TATV_CONFIGURE TATV_CONTAINER TATV_FAIL TATV_FLOW TATV_HTML TATV_NUMERIC_RANGE TATV_PATH TATV_PLAIN TATV_README TATV_TEXT TAT_BASE TAT_CLEARPROFILE TAT_CODEPAGE TAT_CONFIGSYS TAT_CREATEOBJECT TAT_DEEXECUTE TAT_EXECUTE TAT_EXPANDED TAT_EXTERNAL TAT_FIXED TAT_FORMAT TAT_EXTRACTFROMPCK TAT_INDEX TAT_KILLPROCESS TAT_LONGFILENAMES TAT_NAME TAT_NOBASE TAT_NODESELECT TAT_OS TAT_PACKAGEID TAT_REGISTERCLASS TAT_REPLACECLASS TAT_REQUIRES TAT_SELECT TAT_TARGET TAT_TITLE TAT_TYPE TAT_VERSION TAT_WRITEPROFILE T_BODY_BEGIN T_BODY_END T_EQUIV T_GROUP_BEGIN_CLOSE T_GROUP_BEGIN_OPEN T_GROUP_END T_HEAD_BEGIN T_HEAD_END T_MSG_BEGIN T_MSG_END T_NEXTBUTTON_BEGIN_CLOSE T_NEXTBUTTON_BEGIN_OPEN T_NEXTBUTTON_END T_NUMBER T_PAGE_BEGIN_CLOSE T_PAGE_BEGIN_OPEN T_PAGE_END T_PCK_BEGIN_CLOSE T_PCK_BEGIN_OPEN T_PCK_END T_README_BEGIN_CLOSE T_README_BEGIN_OPEN T_README_END T_REXX_OPEN T_REXX_BEGIN_CLOSE T_REXX_END T_STRING T_TEXT_BEGIN T_TEXT_END T_TITLE_BEGIN T_TITLE_END T_VARPROMPT_BEGIN_CLOSE T_VARPROMPT_BEGIN_OPEN T_VARPROMPT_END T_WARPIN_BEGIN_OPEN T_WARPIN_END T_WARPIN_BEGIN_CLOSE // T_COMMENT_BEGIN T_COMMENT_END
%%

wpdocument:
	T_WARPIN_BEGIN_OPEN warpin_attr T_WARPIN_BEGIN_CLOSE T_HEAD_BEGIN head T_HEAD_END T_BODY_BEGIN body T_BODY_END T_WARPIN_END
;

warpin_attr:
	warpin_attr warpin_attr_cont
	|
	warpin_attr_cont
;

warpin_attr_cont:
	TAT_VERSION T_EQUIV T_STRING
	|
	TAT_OS T_EQUIV T_STRING
	|
	TAT_CODEPAGE T_EQUIV T_STRING
;

head:
	head head_elem
	|
	head_elem
;

head_elem:
	group | title | pck | varprompt | msg | rexx
;

body:
	body page
	|
	page
;

group:
	T_GROUP_BEGIN_OPEN group_attr T_GROUP_BEGIN_CLOSE group_pcks T_GROUP_END
;

group_attr:
	TAT_EXPANDED TAT_TITLE T_EQUIV T_STRING
	|
	TAT_TITLE T_EQUIV T_STRING TAT_EXPANDED
	|
	TAT_TITLE T_EQUIV T_STRING
;

group_pcks:
	group_pcks pck
	|
	pck
;

title:
	T_TITLE_BEGIN T_STRING T_TITLE_END
;

varprompt:
	T_VARPROMPT_BEGIN_OPEN varprompt_attr T_VARPROMPT_BEGIN_CLOSE T_STRING T_VARPROMPT_END
	|
	T_VARPROMPT_BEGIN_OPEN varprompt_attr T_VARPROMPT_BEGIN_CLOSE T_VARPROMPT_END
;

varprompt_attr:
	TAT_NAME T_EQUIV T_STRING varprompt_attr_type
	|
	varprompt_attr_type TAT_NAME T_EQUIV T_STRING
;

varprompt_attr_type:
	TAT_TYPE T_EQUIV TATV_ALPHA
	|
	TAT_TYPE T_EQUIV TATV_PATH
	|
	TAT_TYPE T_EQUIV TATV_FAIL
	|
	TAT_TYPE T_EQUIV TATV_NUMERIC_RANGE
;

msg:
	T_MSG_BEGIN T_STRING T_MSG_END
	|
	T_MSG_BEGIN T_MSG_END
;

rexx:
	T_REXX_OPEN TAT_NAME T_EQUIV T_STRING T_REXX_BEGIN_CLOSE T_STRING T_REXX_END
	|
	T_REXX_OPEN TAT_NAME T_EQUIV T_STRING T_REXX_BEGIN_CLOSE T_REXX_END
;

page:
	page_text
	|
	page_readme
	|
	page_container
	|
	page_configure
;

page_text:
	T_PAGE_BEGIN_OPEN page_text_attr T_PAGE_BEGIN_CLOSE text nextbutton T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_text_attr T_PAGE_BEGIN_CLOSE nextbutton text T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_text_attr T_PAGE_BEGIN_CLOSE text T_PAGE_END
;

page_text_attr:
	TAT_INDEX T_EQUIV T_NUMBER TAT_TYPE T_EQUIV TATV_TEXT
	|
	TAT_TYPE T_EQUIV TATV_TEXT TAT_INDEX T_EQUIV T_NUMBER
;

page_readme:
	T_PAGE_BEGIN_OPEN page_readme_attr T_PAGE_BEGIN_CLOSE page_readme_cont T_PAGE_END
;

page_readme_attr:
	TAT_INDEX T_EQUIV T_NUMBER TAT_TYPE T_EQUIV TATV_README
	|
	TAT_TYPE T_EQUIV TATV_README TAT_INDEX T_EQUIV T_NUMBER
;

page_readme_cont:
	text readme nextbutton
	|
	text nextbutton readme
	|
	readme text nextbutton
	|
	readme nextbutton text
	|
	nextbutton text readme
	|
	nextbutton readme text
	|
	readme text
	|
	text readme
;

page_container:
	T_PAGE_BEGIN_OPEN page_container_attr T_PAGE_BEGIN_CLOSE text nextbutton T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_container_attr T_PAGE_BEGIN_CLOSE nextbutton text T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_container_attr T_PAGE_BEGIN_CLOSE text T_PAGE_END
;

page_container_attr:
	TAT_INDEX T_EQUIV T_NUMBER TAT_TYPE T_EQUIV TATV_CONTAINER
	|
	TAT_TYPE T_EQUIV TATV_CONTAINER TAT_INDEX T_EQUIV T_NUMBER
;

page_configure:
	T_PAGE_BEGIN_OPEN page_configure_attr T_PAGE_BEGIN_CLOSE text nextbutton T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_configure_attr T_PAGE_BEGIN_CLOSE nextbutton text T_PAGE_END
	|
	T_PAGE_BEGIN_OPEN page_configure_attr T_PAGE_BEGIN_CLOSE text T_PAGE_END
;

page_configure_attr:
	TAT_INDEX T_EQUIV T_NUMBER TAT_TYPE T_EQUIV TATV_CONFIGURE
	|
	TAT_TYPE T_EQUIV TATV_CONFIGURE TAT_INDEX T_EQUIV T_NUMBER
;

text:
	T_TEXT_BEGIN T_STRING T_TEXT_END
	|
	T_TEXT_BEGIN T_TEXT_END
;

readme:
	T_README_BEGIN_OPEN readme_attr T_README_BEGIN_CLOSE T_STRING T_README_END
	|
	T_README_BEGIN_OPEN readme_attr T_README_BEGIN_CLOSE T_README_END
;

readme_attr:
	TAT_EXTRACTFROMPCK T_EQUIV T_NUMBER readme_attr_format
	|
	readme_attr_format TAT_EXTRACTFROMPCK T_EQUIV T_NUMBER
;

readme_attr_format:
	TAT_FORMAT T_EQUIV TATV_PLAIN
	|
	TAT_FORMAT T_EQUIV TATV_FLOW
	|
	TAT_FORMAT T_EQUIV TATV_HTML
;

nextbutton:
	T_NEXTBUTTON_BEGIN_OPEN TAT_TARGET T_EQUIV T_NUMBER T_NEXTBUTTON_BEGIN_CLOSE T_STRING T_NEXTBUTTON_END
	|
	T_NEXTBUTTON_BEGIN_OPEN TAT_TARGET T_EQUIV T_NUMBER T_NEXTBUTTON_BEGIN_CLOSE T_NEXTBUTTON_END
;

pck:
	T_PCK_BEGIN_OPEN pck_attr T_PCK_BEGIN_CLOSE T_STRING T_PCK_END
	|
	T_PCK_BEGIN_OPEN pck_attr T_PCK_BEGIN_CLOSE T_PCK_END
;

pck_attr:
	pck_attr_opt pck_attr_packageid pck_attr_opt pck_attr_title pck_attr_opt pck_attr_target pck_attr_opt
	|
	pck_attr_opt pck_attr_packageid pck_attr_opt pck_attr_target pck_attr_opt pck_attr_title pck_attr_opt
	|
	pck_attr_opt pck_attr_title pck_attr_opt pck_attr_packageid pck_attr_opt pck_attr_target pck_attr_opt
	|
	pck_attr_opt pck_attr_title pck_attr_opt pck_attr_target pck_attr_opt pck_attr_packageid pck_attr_opt
	|
	pck_attr_opt pck_attr_target pck_attr_opt pck_attr_title pck_attr_opt pck_attr_packageid pck_attr_opt
	|
	pck_attr_opt pck_attr_target pck_attr_opt pck_attr_packageid pck_attr_opt pck_attr_title pck_attr_opt
;

pck_attr_opt:
	pck_attr_opt_cont
	|
	empty
;

pck_attr_opt_cont:
	pck_attr_opt_cont pck_attr_cont
	|
	pck_attr_cont
;

pck_attr_packageid:
	TAT_PACKAGEID T_EQUIV T_STRING
;

pck_attr_title:
	TAT_TITLE T_EQUIV T_STRING
;

pck_attr_target:
	TAT_TARGET T_EQUIV T_STRING
;

pck_attr_cont:
	TAT_BASE
	|
	TAT_NOBASE
	|
	TAT_FIXED
	|
	TAT_SELECT
	|
	TAT_NODESELECT
	|
	TAT_LONGFILENAMES
	|
	TAT_EXTERNAL T_EQUIV T_STRING
	|
	TAT_REQUIRES T_EQUIV T_NUMBER
	|
	TAT_REQUIRES T_EQUIV T_STRING
	|
	TAT_CONFIGSYS T_EQUIV T_STRING
	|
	TAT_REGISTERCLASS T_EQUIV T_STRING
	|
	TAT_REPLACECLASS T_EQUIV T_STRING
	|
	TAT_CREATEOBJECT T_EQUIV T_STRING
	|
	TAT_EXECUTE T_EQUIV T_STRING
	|
	TAT_DEEXECUTE T_EQUIV T_STRING
	|
	TAT_CLEARPROFILE T_EQUIV T_STRING
	|
	TAT_WRITEPROFILE T_EQUIV T_STRING
	|
	TAT_KILLPROCESS T_EQUIV T_STRING
;

empty:;

// comment:
// 	T_COMMENT_BEGIN | T_COMMENT_END
// ;
