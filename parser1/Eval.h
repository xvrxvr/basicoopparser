#pragma once

#include "Tree.h"
#include "MultiMethod.h"

namespace AST_Parse {

	typedef std::map<std::string,NodePtr> VarsMap;

	class EvalVisitor : public Visitor<NodePtr> {
		VarsMap& vars;

		NodePtr reduce(NodeBinOp* v, double left, NodePtr right)
		{
				switch(v->get_opcode())
				{
				case OPC_Add: if (left==0) return right; break;
				case OPC_Sub: if (left==0) return new NodeUnOp(OPC_Neg,right); break;
				case OPC_Mul: if (left==0) return new NodeConst(0);
					          if (left==1) return right;
							  if (left==-1) return new NodeUnOp(OPC_Neg,right);
							  break;
				case 	OPC_Div: if (left==0) return new NodeConst(0); break;
				case 	OPC_LAnd: if (left==0) return new NodeConst(0); break;
				case 	OPC_LOr: if (left==0) return right; break;
				}
				return new NodeBinOp(v->get_opcode(),new NodeConst(left),right);
		}

		NodePtr reduce(NodeBinOp* v, NodePtr left, double right)
		{
				switch(v->get_opcode())
				{
				case OPC_Add: case OPC_Sub: if (right==0) return left; break;
				case OPC_Mul: if (right==0) return new NodeConst(0);
					          if (right==1) return left;
							  if (right==-1) return new NodeUnOp(OPC_Neg,left);
							  break;
				case 	OPC_Div: if (right==1) return left;
							  if (right==-1) return new NodeUnOp(OPC_Neg,left);
							  break;
				case 	OPC_LAnd: if (right==0) return new NodeConst(0); break;
				case 	OPC_LOr: if (right==0) return left; break;
				}
				return new NodeBinOp(v->get_opcode(),left,new NodeConst(right));
		}
		NodePtr reduce(NodeBinOp* v, double l, double r)
		{
				switch(v->get_opcode())
				{
				case OPC_Add: return new NodeConst( l+r);
				case OPC_Sub: return new NodeConst(l-r);
				case OPC_Mul: return new NodeConst(l*r);
				case 	OPC_Div: return new NodeConst(l/r);
				case 	OPC_Eq: return new NodeConst(l==r);
				case 	OPC_Ne: return new NodeConst(l!=r);
				case 	OPC_LAnd: return new NodeConst(l&&r);
				case 	OPC_LOr: return new NodeConst(l||r);
				default: return new NodeConst(0);
				}
		}

	public:
		EvalVisitor(VarsMap& vm) : vars(vm) {}

		virtual NodePtr visit(NodeConst* v) {return v;}
		virtual NodePtr visit(NodeVar* v) 
		{
			if (vars.count(v->get_value())) return vars[v->get_value()];
			return v;
		}
		virtual NodePtr visit(NodeBinOp* v)
		{
			NodePtr left=v->get_child(0)->visit(this);
			NodePtr right=v->get_child(1)->visit(this);
			int sw=(left->get_opcode()==OPC_Const?1:0)+(right->get_opcode()==OPC_Const?2:0);
			switch(sw)
			{
			case 1: return reduce(v,left.as<NodeConst>()->get_value(),right);
			case 2: return reduce(v,left,right.as<NodeConst>()->get_value());
			case 3: return reduce(v,left.as<NodeConst>()->get_value(),right.as<NodeConst>()->get_value());
			}
			return new NodeBinOp(v->get_opcode(),left,right);
		}
		virtual NodePtr visit(NodeUnOp* v)
		{
			NodePtr nd=v->get_child(0)->visit(this);
			if (nd->get_opcode()!=OPC_Const) return new NodeUnOp(v->get_opcode(),nd);
			double val=nd.as<NodeConst>()->get_value();
			switch(v->get_opcode())
			{
			case OPC_Neg: return new NodeConst(- val);
			case OPC_Inv: return new NodeConst( ~ (unsigned long)val);
			case OPC_LogInv: return new NodeConst(!val);
			default: return new NodeConst(0);
			}

		}
//		virtual NodePtr visit(FuncOp *v) {return v;}
		VISIT_FUNC(NodePtr)

	};


}
