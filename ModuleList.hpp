// ModuleList.hpp: interface for the ModuleList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MODULELIST_HPP_INCLUDED
#define MODULELIST_HPP_INCLUDED

#pragma warning(disable : 4786)

#include <map>
#include <string>

class Module;

class ModuleList  
{
public:
	typedef std::map<std::string, Module*> ModulesT;

private:
	ModulesT modules;

public:
	ModuleList();
	virtual ~ModuleList();

	void addModule(const std::string& name, Module* module);
	Module* lookup(const std::string& name);
	bool contains(const std::string& name);

};

#endif // !defined(AFX_MODULELIST_HPP__26A58A16_0909_4857_8C29_2F5A0AA970BA__INCLUDED_)
