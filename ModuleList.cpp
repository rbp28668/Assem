// ModuleList.cpp: implementation of the ModuleList class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "ModuleList.hpp"
#include "Module.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModuleList::ModuleList()
{

}

ModuleList::~ModuleList()
{
	for(ModulesT::iterator iter = modules.begin();
	iter != modules.end();
	++iter)
	{
		Module* module = iter->second;
		delete module;
	}
}

void ModuleList::addModule(const std::string& name, Module* module)
{
	assert(this);
	assert(module);

	modules.insert(ModulesT::value_type(name,module));
}

Module* ModuleList::lookup(const std::string& name)
{
	assert(this);
	ModulesT::iterator iter = modules.find(name);
	Module* module = 0;
	if(iter != modules.end())
	{
		module = iter->second;
	}
	return module;
}

bool ModuleList::contains(const std::string& name)
{
	assert(this);
	return lookup(name) != 0;
}