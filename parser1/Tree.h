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

		template<class Type>
		Type* as() {return (Type*)ptr;}
		template<class Type>
		const Type* as() const {return (Type*)ptr;}

	};

	class NodeConst;
	class NodeVar;
	class NodeUnOp;
	class NodeBinOp;
	class FuncOp;

	template<class Ret>
	class Visitor {
	public:
		virtual Ret visit(NodeConst*)=0;
		virtual Ret visit(NodeVar*)=0;
		virtual Ret visit(NodeBinOp*)=0;
		virtual Ret visit(NodeUnOp*)=0;
		virtual Ret visit(FuncOp*) =0;
	};

#define VISIT_DEF \
		virtual void visit(Visitor<void>*) =0; \
		virtual std::string visit(Visitor<std::string>*) =0; \
		virtual NodePtr visit(Visitor<NodePtr>*) =0; \
		virtual double visit(Visitor<double>*)=0;

#define VISIT \
	virtual void visit(Visitor<void>* v) {v->visit(this);} \
		virtual std::string visit(Visitor<std::string>* v) {return v->visit(this);} \
		virtual NodePtr visit(Visitor<NodePtr>* v) {return v->visit(this);} \
		virtual double visit(Visitor<double>* v) {return v->visit(this);}

#define VISIT_FUNC(Ret) \
		virtual Ret visit(FuncOp* func) \
	     { \
		  return MultiMethod<Ret>::get_instance().call(func->get_func(),this); \
	     }

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

		VISIT_DEF
	};

	class NodeConst : public ASTNode {
		const double value;
	public:
		NodeConst(double val) : ASTNode(OPC_Const), value(val) {}
		virtual size_t get_child_count() const {return 0;}
		virtual NodePtr get_child(size_t index) const {return NodePtr();}

		VISIT

		double get_value() const {return value;}
	};

	class NodeVar : public ASTNode {
		const std::string var_name;
	public:
		NodeVar(const std::string& n) : ASTNode(OPC_Var), var_name(n) {}
		virtual size_t get_child_count() const {return 0;}
		virtual NodePtr get_child(size_t index) const {return NodePtr();}
		VISIT
		std::string get_value() const {return var_name;}
	};

	class NodeUnOp : public ASTNode {
		NodePtr child;
	public:
		NodeUnOp(OpCode op, NodePtr c) : ASTNode(op), child(c) {}
		virtual size_t get_child_count() const {return 1;}
		virtual NodePtr get_child(size_t index) const {return child;}
		VISIT
	};

	class NodeBinOp : public ASTNode {
		NodePtr left, right;
	public:
		NodeBinOp(OpCode op, NodePtr l, NodePtr r) : ASTNode(op), left(l), right(r) {}
		virtual size_t get_child_count() const {return 2;}
		virtual NodePtr get_child(size_t index) const {return index?right:left;}
		VISIT
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
		VISIT

		GenericFunc* get_func() const {return func_id;}
	};

}
