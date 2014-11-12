#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "EvalSystem.h"
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
	EvalSystem<> es;
	for(;;)
	{
		printf("> ");
		gets(buf);
		auto ex= es.compile(buf);
		std::string res=ex.to_string();
		std::string ev=ex.eval().to_string();
		printf("%s = %s (%s)\n",buf,ev.c_str(), res.c_str());
	}
	return 0;
}
