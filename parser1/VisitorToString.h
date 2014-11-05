#pragma once

#include "Tree.h"
#include "func.h"

namespace AST_Parse {

	class VisitorToString : public Visitor<std::string> {
		int prio;
	public:

		virtual std::string visit(NodeConst* v)
		{
			char b[128];
			sprintf(b,"%g",v->get_value());
			prio=1000;
			return b;
		}

		virtual std::string visit(NodeVar* v)
		{
			prio=1000;
			return v->get_value();
		}

		virtual std::string visit(NodeBinOp* v)
		{
	     static const int p[]={0,4,4,5,5,1,1,2,3};
	     static const char* ids[]={"","+","-","*","/","==","!=","&&","||"};

		 std::string l=v->get_child(0)->visit(this);
		 int l_prio=prio;
		 
		 std::string r=v->get_child(1)->visit(this);
		 int r_prio=prio;

		 prio=p[v->get_opcode()&0xFF];
		 if (l_prio < prio) l="("+l+")";
		 if (r_prio <= prio) r="("+r+")";
	     return l+ids[v->get_opcode()&0xFF] + r;
		}

		virtual std::string visit(NodeUnOp* v)
		{
	     static const char* ids[]={"","-","~","!"};

		 std::string s=v->get_child(0)->visit(this);
		 int ch_prio=prio;

		 prio=999;
	     if (ch_prio <= prio) s="("+s+")";
	     return ids[v->get_opcode()&0xFF] + s;
		}

//		VISIT_FUNC(std::string)

		virtual std::string visit(FuncOp* func)
		{
			std::string rv=func->get_func()->get_id();
			rv+="(";
			for(size_t i=0;i<func->get_child_count();++i)
			{
				rv+=func->get_child(i)->visit(this);
				if (i+1<func->get_child_count()) rv+=",";
			}
			return rv+")";
		}

	};

}
