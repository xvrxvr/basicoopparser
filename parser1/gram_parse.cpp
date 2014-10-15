#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "gram_parse.h"
#include "func.h"

namespace AST_Parse {

#include "gramm.tab.h"

int BisonParser::yylex(void* rv)
{
	while(*input)
	{
		if (isspace(*input)) {++input; continue;}
		if (isdigit(*input))
		{
			*(double*)rv=strtod(input,(char**)&input);
			return vNUM;
		}
		if (isalnum(*input))
		{
			const char* start=input;
			while(isalnum(*input)) ++input;
			char* mem=new char[input-start+1]; //!!! Memory leak - fix me !!!
			memcpy(mem,start,input-start);
			mem[input-start]=0;
			*(char**)rv=mem;
			return vID;
		}
#define V2(s1,s2) ((s1)*256+(s2))
		switch(V2(input[0],input[1]))
		{
		case V2('=','='): input+=2; return tEQ;
		case V2('!','='): input+=2; return tNE;
		case V2('&','&'): input+=2; return tAND;
		case V2('|','|'): input+=2; return tOR;
		}
#undef V2
		return *input++;
	}
	return 0;
}

/*
double BisonParser::get_var_value(std::string id)
{
			if (vars.count(id)==0)
			{
				yyerror(("Variable '"+id+"' not found").c_str());
				return 0;
			}
			else
			 return vars[id];

}
*/


		FuncOp::FuncOp(const char* id, ParamList* pl) : ASTNode(OPC_Func) 
		{
			while(pl)
			{
				args.push_back(pl->expr);
				ParamList* pp=pl;
				pl=pl->next;
				delete pp;
			}
			func_id=GF_Fabric::create(id);
			if (!func_id) 
			{
				printf("Error: Function '%s' not found\nHACK HACK HACK!!!!\n",id);
				throw std::exception();
			}
		}

GF_Fabric* GF_Fabric::root;

double FuncOp::eval() const
{
	return func_id->eval(args);
}


void BisonParser::yyerror(const char* msg)
{
	fprintf(stderr,"%s\n",msg);
}

#define yyparse BisonParser::yyparse

#include "gramm.tab.c"
}
