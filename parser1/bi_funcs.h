#pragma once

#include "Tree.h"
#include "MultiMethod.h"
#include "func.h"
#include "Eval.h"

namespace AST_Parse {

enum FuncsIdx {
	FI_SIN,
	FI_COS,
	FI_TG
};

class Func_bi : public GenericFunc {
	int idx;
public:
	Func_bi(int fidx) {idx=fidx-1;}

	virtual std::string get_id() const 
	{
		static const char* ids[]={
			"sin",
			"cos",
			"tg"
		};
		return ids[idx];
	}
	
	int get_idx() const {return idx;}
};
}
