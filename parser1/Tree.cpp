#include "Tree.h"

namespace AST_Parse {

void NodePtr::assign(ASTNode* p)
{
	if (p)
	{
		++p->ref_count;
	}
	if (ptr)
	{
		if (!--ptr->ref_count) delete ptr;
	}
	ptr=p;
}



double NodeUnOp::eval() const
{
	switch(get_opcode())
	{
	case OPC_Neg: return - child->eval();
	case OPC_Inv: return ~ (unsigned long)child->eval();
	case OPC_LogInv: return !child->eval();
	default: return 0;
	}
}

double NodeBinOp::eval() const
{
	switch(get_opcode())
	{
	case OPC_Add: return left->eval()+right->eval();
	case OPC_Sub: return left->eval()-right->eval();
	case OPC_Mul: return left->eval()*right->eval();
	case 	OPC_Div: return left->eval()/right->eval();
	case 	OPC_Eq: return left->eval()==right->eval();
	case 	OPC_Ne: return left->eval()!=right->eval();
	case 	OPC_LAnd: return left->eval()&&right->eval();
	case 	OPC_LOr: return left->eval()||right->eval();
	default: return 0;
	}
}


}
