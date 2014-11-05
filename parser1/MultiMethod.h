#pragma once
#include "Tree.h"
#include <map>

template<class Ret>
class MultiMethod {
	class CallPeer {
	public:
		virtual Ret call(AST_Parse::GenericFunc*, AST_Parse::Visitor<Ret>*)=0;
	};
	template<class Ret, class LeftT, class RightT>
	class CallPeerImp : public CallPeer {
		Ret (*func)(LeftT*,RightT*);
	public:
		CallPeerImp(Ret (*f)(LeftT*,RightT*)) : func(f) {}
		virtual Ret call(AST_Parse::GenericFunc* left, AST_Parse::Visitor<Ret>* right)
		{
			return func((LeftT*)left,(RightT*)right);
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
	void reg(Ret (*func) (Left*, Right*))
	{
		mm_map[std::string(typeid(Left).name())+"|"+typeid(Right).name()]=new CallPeerImp<Ret,Left,Right>(func);
	}

	Ret call(AST_Parse::GenericFunc* left, AST_Parse::Visitor<Ret>* right)
	{
		std::string nm=std::string(typeid(*left).name())+"|"+typeid(*right).name();
		if (!mm_map.count(nm))
		{
			printf("MMETHOD (%s): Unregietered!\n",nm.c_str());
			return Ret();
		}
		return mm_map[nm]->call(left,right);
	}
};
