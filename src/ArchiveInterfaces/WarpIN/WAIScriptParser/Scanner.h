#ifndef Scanner_H_INCLUDED_
#define Scanner_H_INCLUDED_

#include "Scannerbase.h"
#include <stack>

class Scanner: public ScannerBase{
public:
	explicit Scanner(std::istream &in=std::cin,std::ostream &out=std::cout);
	Scanner(std::string const &infile,std::string const &outfile);
	int lex();

private:
	int lex__();
	int executeAction__(size_t ruleNr);

	std::stack<StartCondition__> conditionsStack;
	void push_state(StartCondition__ startCondition);
	void pop_state();

	void print();
	void preCode(); // re-implement this function for code that must 
					// be exec'ed before the patternmatching starts

	void postCode(PostEnum__ type);    
					// re-implement this function for code that must 
					// be exec'ed after the rules's actions.
};

inline Scanner::Scanner(std::istream &in,std::ostream &out)
:ScannerBase(in,out){}

inline Scanner::Scanner(std::string const &infile,std::string const &outfile)
:ScannerBase(infile,outfile){}

inline int Scanner::lex(){
	return lex__();
}

inline void Scanner::preCode(){
	// optionally replace by your own code
}

inline void Scanner::postCode(PostEnum__ type){
	// optionally replace by your own code
}

inline void Scanner::print(){
	print__();
}

inline void Scanner::push_state(StartCondition__ startCondition){
	this->conditionsStack.push(this->startCondition());
	this->begin(startCondition);
}

inline void Scanner::pop_state(){
	if(!this->conditionsStack.empty()){
		this->begin(this->conditionsStack.top());
		this->conditionsStack.pop();
	}
	else this->begin(StartCondition__::INITIAL);
}

#endif // Scanner_H_INCLUDED_
