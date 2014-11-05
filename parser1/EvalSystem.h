#pragma once

#include <Windows.h>

#include "Tree.h"
#include "gram_parse.h"
#include "VisitorToString.h"
#include "Eval.h"

namespace AST_Parse { namespace Utils {

class LockMgr_MT {
	CRITICAL_SECTION cs;
public:
	LockMgr_MT()
	{
		InitializeCriticalSection(&cs);
	}
	~LockMgr_MT()
	{
		DeleteCriticalSection(&cs);
	}

	void lock()
	{
		EnterCriticalSection(&cs);
	}
	void unlock()
	{
		LeaveCriticalSection(&cs);
	}
};

class LockMgr_Dummy {
public:
	void lock() {}
	void unlock() {}
};

template<class LM>
class Lock {
	LM &M;
public:
	Lock(LM& mgr) : M(mgr) {M.lock();}
	~Lock() {M.unlock();}
};

#ifdef _MT
typedef LockMgr_MT LockMgr_Def;
#else
typedef LockMgr_Dummy LockMgr_Def;
#endif

}}


template<class LM = AST_Parse::Utils::LockMgr_Def>
class EvalSystem {
	AST_Parse::VarsMap vars;
	class N;
	friend class N;

	typedef LM LockMgr;
	typedef AST_Parse::Utils::Lock<LM> LLock;
	LockMgr lock_mgr;

public:
	class N : public AST_Parse::NodePtr {
		EvalSystem* own;
		N(EvalSystem* self, AST_Parse::NodePtr p) :AST_Parse::NodePtr(p), own(self) {}
		friend class EvalSystem;
	public:

		N eval()
	     {
	      LLock l(own->lock_mgr);
		  AST_Parse::EvalVisitor eval(own->vars);
	      return N(own,(*this)->visit(&eval));
	     }

		std::string to_string()
		{
	     LLock l(own->lock_mgr);
		 AST_Parse::VisitorToString vis;
		 return (*this)->visit(&vis);
		}

		class NPtr {
			LockMgr* lmgr;
			AST_Parse::ASTNode* node;
		public:
			NPtr(LockMgr* l, AST_Parse::ASTNode* n) : lmgr(l), node(n)
			{
				lmgr->lock();
			}
			~NPtr() {lmgr->unlock();}
			NPtr(const NPtr& n) : lmgr(n.lmgr), node(n.node)
			{
				lmgr->lock();
			}
			NPtr& operator=(const NPtr& n)
			{
				n.lmgr->lock();
				lmgr->unlock();
				lmgr=n.lmgr;
				node=n.node;
			}
			AST_Parse::ASTNode* operator -> () {return node;}
		};

		NPtr operator -> ()
		{
			return NPtr(&own->lock_mgr, as<AST_Parse::ASTNode>());
		}

	};


	bool has_var(std::string name) 
	 {
		LLock l(lock_mgr);
		return vars.count(name)!=0;
	}
	AST_Parse::NodePtr get_var(std::string name) 
	{
		LLock l(lock_mgr);
		if (has_var(name)) return vars[name];
		return 0;
	}
	void set_var(std::string name, AST_Parse::NodePtr p) 
	{
		LLock l(lock_mgr);
		vars[name]=p;
	}

	N compile(std::string val)
	{
	 LLock l(lock_mgr);
	 AST_Parse::BisonParser BP;
	 return N(this,BP.parse(val.c_str()));
	}

};
