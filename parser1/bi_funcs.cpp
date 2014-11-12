#include "bi_funcs.h"
#include <math.h>

namespace AST_Parse {

FUNC_VL(Func_bi,"sin,cos,tg");

MM_FUNC(NodePtr, Func_bi, EvalVisitor)
{
	NodePtr v1=op->get_child(0)->visit(visitor);
	if (v1->get_opcode()!=OPC_Const) return new FuncOp(func,v1);
	double val=v1.as<NodeConst>()->get_value();
	switch(func->get_idx())
	{
	case FI_SIN: val=sin(val); break;
	case FI_COS: val=cos(val); break;
	case FI_TG: val=tan(val); break;
	}
	return new NodeConst(val);
}
}
