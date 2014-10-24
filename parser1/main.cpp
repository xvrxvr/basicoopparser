#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "gram_parse.h"
#include "VisitorToString.h"
#include "Eval.h"

/*

expr: mul_exp { '+'|'-'  mul_exp }
mul_exp: primary { '*'|'/' primary }
primary: <number> | '(' expr ')'

*/

class Lexer {
	char* input;
	int num;
	int symbol;
public:
	Lexer(char* in) : input(in), num(0), symbol(-1) {}
	void init(char* in) {input=in; num=0; symbol=-1;}

	int lex(int& num)
	{
	while(*input)
	{
		if (isspace(*input)) {++input; continue;}
		if (isdigit(*input))
		{
			num=strtol(input,&input,0);
			return 'N';
		}
		return *input++;
	}
	return 0;
	}

	int peek_sym(int& onum)
	{
		onum=num;
		if (symbol==-1) {symbol=lex(onum); num=onum;}
		return symbol;
	}

	void next() {symbol=-1;}
};

///////////////////////////////////////////////////////////////

int expr(Lexer& L);

int primary(Lexer& L)
{
	int num;
	if (L.peek_sym(num)=='N')
	{
		L.next();
		return num;
	}
	if (L.peek_sym(num)=='(')
	{
		L.next();
		int rv=expr(L);
		if (L.peek_sym(num)!=')')
		{
			printf("Error!\n");
			abort();
		}
		L.next();
		return rv;
	}
	printf("Error!\n");
	abort();
}

//mul_exp: primary { '*'|'/' primary }
int mul_expr(Lexer& L)
{
	int num,rv=primary(L);
	for(;;)
	{
		if (L.peek_sym(num)=='*') {L.next(); rv*=primary(L);} else
		if (L.peek_sym(num)=='/') {L.next(); rv/=primary(L);}
		else return rv;
	}
}

//expr: mul_exp { '+'|'-'  mul_exp }
int expr(Lexer& L)
{
	int num,rv=mul_expr(L);
	for(;;)
	{
		if (L.peek_sym(num)=='+') {L.next(); rv+=mul_expr(L);} else
		if (L.peek_sym(num)=='=') {L.next(); rv-=mul_expr(L);}
		else return rv;
	}
}

char buf[1024];

int main()
{
	AST_Parse::BisonParser BP;
	for(;;)
	{
		printf("> ");
		gets(buf);
//		Lexer L(buf);
		AST_Parse::NodePtr ex= BP.parse(buf);
		AST_Parse::VisitorToString vis;
		AST_Parse::EvalVisitor eval;
		std::string res=ex->visit(&vis);
		std::string ev=ex->visit(&eval)->visit(&vis);
		printf("%s = %s (%s)\n",buf,/*expr(L)*/ ev.c_str(), res.c_str());
	}
	return 0;
}

using namespace AST_Parse;

class Func_sin : public GenericFunc {
public:
	Func_sin(int) {}

	virtual std::string get_id() const {return "sin";}
	
};
FUNC(sin);
