#include "diff.h"
#include "bi_funcs.h"
#include "gram_parse.h"

namespace AST_Parse {

MM_FUNC(NodePtr, Func_bi, DiffVisitor)
{
	const char* f[]={
		"@1'*cos(@1)",
		"-@1'*sin(@1)",
		"@1'/(cos(@1)*cos(@1))"
	};
	return DoDiff(f[func->get_idx()],visitor->get_dvar()).add_arg(op->get_child(0)).result();
}

DoDiff& DoDiff::add_arg(NodePtr arg)
{
	char n[2]={vars.size()+'1',0};
	std::string s="@";
	s+=n;

	vars[s]=arg;
	DiffVisitor dv(dvar);
	vars[s+"'"]=arg->visit(&dv);
	return *this;
}

NodePtr DoDiff::result()
{
	BisonParser bp(func,"@'");
	NodePtr p=bp.parse();
	EvalVisitor ev(vars);
	return p->visit(&ev);
}

}

