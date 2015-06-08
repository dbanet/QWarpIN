#ifndef Parser_h_included
#define Parser_h_included

#include "ArchiveInterfaces/WarpIN/WAIScriptParser/WAIScriptParser.h"
#include "Parserbase.h"
#include "Scanner.h"

#undef Parser
class Parser: public ParserBase{
public:
    Parser(std::istream &in,WAIScriptParser *parser);
    int parse();

private:
    Scanner scanner;
    WAIScriptParser *out;
    void error(char const *msg); // called on (syntax) errors
    int lex();                   // returns the next token from the
                                 // lexical scanner.
    void print();                // use, e.g., d_token, d_loc

    // support functions for parse():
    void executeAction(int ruleNr);
    void errorRecovery();
    int lookup(bool recovery);
    void nextToken();
    void print__();
    void exceptionHandler__(std::exception const &exc);

};

#endif
