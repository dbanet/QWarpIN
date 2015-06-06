@echo off
echo #include ^<iostream^>>__main.cpp
echo #include ^"Scanner.h^">>__main.cpp
echo using namespace std;>>__main.cpp
echo int main(){>>__main.cpp
echo	Scanner scanner;>>__main.cpp
echo	while(scanner.lex())>>__main.cpp
echo		cout^<^<^"[Identifier: ^"^<^<scanner.matched()^<^<^"]^";>>__main.cpp
echo }>>__main.cpp
(cat lexer.ll) | ((sed "s/return \([^;]*\)/std\:\:cout\<\<\"\1\"\<\<std\:\:endl/") >__lexer.ll) && (flexc++ __lexer.ll) && (g++ --std=c++14 lex.cc __main.cpp -o __test.exe) && (rm __main.cpp __lexer.ll lex.cc Scannerbase.h) && (echo go) && (.\__test.exe) & (rm __test.exe)