#pragma once

#include <string>
#include <vector>

namespace AST_Parse {

	enum OpCode {
		OPC_Const,
		OPC_Var,
		OPC_Func,

		OPC_UnOp = 0x1000,
		OPC_Neg,
		OPC_Inv,
		OPC_LogInv,

		OPC_BinOp = 0x2000,
		OPC_Add,
		OPC_Sub,
		OPC_Mul,
		OPC_Div,
		OPC_Eq,
		OPC_Ne,
		OPC_LAnd,
		OPC_LOr
	};

	class ASTNode;

	class NodePtr {
		ASTNode* ptr;

		void assign(ASTNode* p);
	public:
		NodePtr(ASTNode* p=0) :ptr(0) {assign(p);}
		~NodePtr() {assign(0);}

		NodePtr(const NodePtr& p) : ptr(0) {assign(p.ptr);}
		NodePtr& operator =(const NodePtr&p) {assign(p.ptr); return *this;}

		ASTNode* operator -> () const {return ptr;}
	};

	class NodeConst;
	class NodeVar;
	class NodeUnOp;
	class NodeBinOp;
	class FuncOp;

	class Visitor {
	public:
		virtual void visit(NodeConst*)=0;
		virtual void visit(NodeVar*)=0;
		virtual void visit(NodeBinOp*)=0;
		virtual void visit(NodeUnOp*)=0;
		virtual void visit(FuncOp*) {}
	};

	class ASTNode {
		const OpCode opcode;
		size_t ref_count;
		friend NodePtr;
	public:
		ASTNode(OpCode opc) : opcode(opc) {}
		virtual ~ASTNode() {}

		OpCode get_opcode() const {return opcode;}

		virtual size_t get_child_count() const =0;
		virtual NodePtr get_child(size_t index) const =0;

		virtual void visit(Visitor*) =0;

		virtual double eval() const =0;
	};

	class NodeConst : public ASTNode {
		const double value;
	public:
		NodeConst(double val) : ASTNode(OPC_Const), value(val) {}
		virtual size_t get_child_count() const {return 0;}
		virtual NodePtr get_child(size_t index) const {return NodePtr();}

		virtual void visit(Visitor *v) {v->visit(this);}

		double get_value() const {return value;}

		virtual double eval() const {return value;}
	};

	class NodeVar : public ASTNode {
		const std::string var_name;
	public:
		NodeVar(const std::string& n) : ASTNode(OPC_Var), var_name(n) {}
		virtual size_t get_child_count() const {return 0;}
		virtual NodePtr get_child(size_t index) const {return NodePtr();}
		virtual void visit(Visitor *v) {v->visit(this);}
		std::string get_value() const {return var_name;}

		virtual double eval() const {return 0;}
	};

	class NodeUnOp : public ASTNode {
		NodePtr child;
	public:
		NodeUnOp(OpCode op, NodePtr c) : ASTNode(op), child(c) {}
		virtual size_t get_child_count() const {return 1;}
		virtual NodePtr get_child(size_t index) const {return child;}
		virtual void visit(Visitor *v) {v->visit(this);}

		virtual double eval() const;
	};

	class NodeBinOp : public ASTNode {
		NodePtr left, right;
	public:
		NodeBinOp(OpCode op, NodePtr l, NodePtr r) : ASTNode(op), left(l), right(r) {}
		virtual size_t get_child_count() const {return 2;}
		virtual NodePtr get_child(size_t index) const {return index?right:left;}
		virtual void visit(Visitor *v) {v->visit(this);}

		virtual double eval() const;
	};

	struct ParamList {
		ASTNode* expr;
		ParamList* next;

		ParamList(ASTNode* n) {expr=n; next=NULL;}
		ParamList* join(ParamList* l)
		{
			ParamList* p=this;
			while(p->next) p=p->next;
			p->next=l;
			return this;
		}
	};

	class GenericFunc;

	class FuncOp : public ASTNode {
		GenericFunc* func_id;
		std::vector<NodePtr> args;

	public:
		FuncOp(const char* id, ParamList* pl);

		virtual size_t get_child_count() const {return args.size();}
		virtual NodePtr get_child(size_t index) const {return args[index];}
		virtual void visit(Visitor *v) {v->visit(this);}

		virtual double eval() const;

		GenericFunc* get_func() const {return func_id;}
	};

}
