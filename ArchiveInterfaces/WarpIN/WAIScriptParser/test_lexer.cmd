@echo off
(cat lexer.ll) | ((sed "s/return \([^;]*\)/std\:\:cout\<\<\"\1\"\<\<std\:\:endl/") >__lexer.ll) && (flex __lexer.ll) && (g++ lex.yy.cc) && (rm __lexer.ll lex.yy.cc) && (echo go) && (lex.yy)