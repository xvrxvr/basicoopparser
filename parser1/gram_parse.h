#pragma once

#include <map>
#include <string>

#include "Tree.h"

namespace AST_Parse {

class BisonParser {
	NodePtr result;
	int yylex(void*);
	int yyparse();

	const char* input;
//	std::map<std::string,double> vars;

//	double get_var_value(std::string s);
	void set_var_value(std::string id, NodePtr v) {} //{vars[id]=v;}

public:

	BisonParser(const char* str=0) : input(str) {}

	NodePtr parse(const char* str=NULL) {if (str) input=str; yyparse(); return result;}

	virtual void yyerror(const char* str);
};
}
