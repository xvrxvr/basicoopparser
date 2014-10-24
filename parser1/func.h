#pragma once

#include "Tree.h"

namespace AST_Parse {

class GenericFunc {
public:
	virtual ~GenericFunc() {}

	virtual std::string get_id() const =0;
};

class GF_Fabric {
	static GF_Fabric* root;
	GF_Fabric* next;
public:
	GF_Fabric() {next=root; root=this;}
	virtual ~GF_Fabric() {}

	virtual GenericFunc* create(int) =0;
	virtual int is_our(const char* id) =0;

	static GenericFunc* create(const char* id)
	{
		for(GF_Fabric* f=root; f!=NULL; f=f->next)
			if (int data=f->is_our(id)) return f->create(data);
		return NULL;
	}
};

template<class Item>
class GF_FabricImp : public GF_Fabric {
	std::vector<std::string> ids;
public:
	GF_FabricImp(const char* i) 
	{
		char* pp=strdup(i);
		for(char* id=strtok(pp,",");id;id=strtok(NULL,","))
			ids.push_back(id);
		free(pp);
	}

	virtual GenericFunc* create(int data) {return new Item(data);}
	virtual int is_our(const char* fid) 
	 {
		 std::vector<std::string>::iterator iter=std::find(ids.begin(),ids.end(),std::string(fid));
		 return iter==ids.end()?0:iter-ids.begin()+1;
	 }

};
#define FUNC_VL(nm,lst) static GF_FabricImp<nm> class_fabric_instance_of_##nm (lst)
#define FUNC(nm) FUNC_VL(Func_##nm,#nm)


}
