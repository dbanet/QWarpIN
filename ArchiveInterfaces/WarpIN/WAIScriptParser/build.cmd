@echo off
flex -o lexer.cpp lexer.ll
bison --language=c++ --defines=parser.h -o parser.cpp parser.yy