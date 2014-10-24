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



}
