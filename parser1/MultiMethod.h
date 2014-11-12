#pragma once
#include "Tree.h"
#include <map>

template<class Ret>
class MultiMethod {
	class CallPeer {
	public:
		virtual Ret call(AST_Parse::FuncOp*,AST_Parse::GenericFunc*, AST_Parse::Visitor<Ret>*)=0;
	};
	template<class Ret, class LeftT, class RightT>
	class CallPeerImp : public CallPeer {
		Ret (*func)(AST_Parse::FuncOp*,LeftT*,RightT*);
	public:
		CallPeerImp(Ret (*f)(AST_Parse::FuncOp*,LeftT*,RightT*)) : func(f) {}
		virtual Ret call(AST_Parse::FuncOp* f, AST_Parse::GenericFunc* left, AST_Parse::Visitor<Ret>* right)
		{
			return func(f,(LeftT*)left,(RightT*)right);
		}
	};

	std::map<std::string,CallPeer*> mm_map;

	MultiMethod() {}

public:
	static MultiMethod<Ret>& get_instance()
	{
		static MultiMethod<Ret> rv;
		return rv;
	}

	template<class Left, class Right>
	void reg(Ret (*func) (AST_Parse::FuncOp*,Left*, Right*))
	{
		mm_map[std::string(typeid(Left).name())+"|"+typeid(Right).name()]=new CallPeerImp<Ret,Left,Right>(func);
	}

	Ret call(AST_Parse::FuncOp* func, AST_Parse::GenericFunc* left, AST_Parse::Visitor<Ret>* right)
	{
		std::string nm=std::string(typeid(*left).name())+"|"+typeid(*right).name();
		if (!mm_map.count(nm))
		{
			printf("MMETHOD (%s): Unregietered!\n",nm.c_str());
			return Ret();
		}
		return mm_map[nm]->call(func,left,right);
	}
};

template<class Ret, class Left, class Right, Ret (Func)(AST_Parse::FuncOp*,Left*, Right*)>
class MMReg {
public:
	MMReg()
	{
		MultiMethod<Ret>::get_instance().reg(Func);
	}
};

#define MM_FUNC(Ret, FuncType, VisitorType) \
Ret mm_function_of_##Ret##_##FuncType##_##VisitorType(AST_Parse::FuncOp*,FuncType*,VisitorType*); \
static MMReg<Ret,FuncType,VisitorType,mm_function_of_##Ret##_##FuncType##_##VisitorType> rigister_of_mm_function_of_##Ret##_##FuncType##_##VisitorType; \
Ret mm_function_of_##Ret##_##FuncType##_##VisitorType(AST_Parse::FuncOp* op, FuncType* func, VisitorType* visitor)