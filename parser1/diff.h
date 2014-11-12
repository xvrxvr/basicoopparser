#pragma once

#include "Tree.h"
#include "MultiMethod.h"
#include "Eval.h"

namespace AST_Parse {

	class DoDiff {
		const char* func;
		std::string dvar;
		VarsMap vars;

	public:
		DoDiff(const char*f, std::string d) : func(f), dvar(d) {}

		DoDiff& add_arg(NodePtr);
		NodePtr result();
	};

	class DiffVisitor : public Visitor<NodePtr> {
		std::string dvar;
	public:
		DiffVisitor(std::string v) : dvar(v) {}
		std::string get_dvar() {return dvar;}

		virtual NodePtr visit(NodeConst*) {return new NodeConst(0);}
		virtual NodePtr visit(NodeVar* var)
		{
			return new NodeConst(var->get_value()==dvar?1:0);
		}

		virtual NodePtr visit(NodeBinOp *op)
		{
			const char* f[]={
				"@1' + @2'", //	OPC_Add,
				"@1' - @2'", //OPC_Sub,
				"@1'*@2 + @1*@2'", //OPC_Mul,
				"(@1'*@2-@1*@2')/(@2*@2)", //OPC_Div,
			};
			if (op->get_opcode()>=OPC_Add && op->get_opcode()<=OPC_Div)
			{
				return DoDiff(f[op->get_opcode()-OPC_Add],dvar).add_arg(op->get_child(0)).add_arg(op->get_child(1)).result();
			}
			return new NodeConst(0);
		}

		virtual NodePtr visit(NodeUnOp* op)
		{
			if (op->get_opcode()>=OPC_Neg && op->get_opcode()<=OPC_Inv)
			{
				return DoDiff("-@1'",dvar).add_arg(op->get_child(0)).result();
			}
			return new NodeConst(0);
		}

		VISIT_FUNC(NodePtr);
	};


}
