#pragma once

#include "Tree.h"

namespace AST_Parse {

	class VisitorToString : public Visitor {
		int prio;
	public:
		std::string res;

		virtual void visit(NodeConst* v)
		{
			char b[128];
			sprintf(b,"%g",v->get_value());
			prio=1000;
			res=b;
		}

		virtual void visit(NodeVar* v)
		{
			prio=1000;
			res=v->get_value();
		}

		virtual void visit(NodeBinOp* v)
		{
	     static const int p[]={0,4,4,5,5,1,1,2,3};
	     static const char* ids[]={"","+","-","*","/","==","!=","&&","||"};

		 v->get_child(0)->visit(this);
		 std::string l=res;
		 int l_prio=prio;
		 
		 v->get_child(1)->visit(this);
		 std::string r=res;
		 int r_prio=prio;

		 prio=p[v->get_opcode()&0xFF];
		 if (l_prio < prio) l="("+l+")";
		 if (r_prio <= prio) r="("+r+")";
	     res=l+ids[v->get_opcode()&0xFF] + r;
		}

		virtual void visit(NodeUnOp* v)
		{
	     static const char* ids[]={"","-","~","!"};

		 v->get_child(0)->visit(this);
		 std::string s=res;
		 int ch_prio=prio;

		 prio=999;
	     if (ch_prio <= prio) s="("+s+")";
	     res=ids[v->get_opcode()&0xFF] + s;
		}

	};

}
