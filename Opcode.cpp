// Opcode.cpp: implementation of the Opcode class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "Opcode.hpp"
#include "ParameterList.hpp"
#include "Production.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Opcode::Opcode(char* name) 
: opcode(name) 
, production(0)
{
	assert(name);
}

Opcode::~Opcode()
{
	for(PListListT::iterator iter = args.begin();
	iter != args.end();
	++iter)
	{
		ParameterList* plist = *iter;
		delete plist;
	}
	delete production;
}

const std::string& Opcode::getName() const 
{ 
	assert(this);
	return opcode;
}

const bool Opcode::hasArgs() const 
{
	assert(this);
	return args.size() > 0;
}

const Opcode::PListListT& Opcode::getArgs() const 
{
	assert(this);
	return args;
}

const Production& Opcode::getProduction() const 
{
	assert(this);
	assert(production);
	return *production;
}

void Opcode::setProduction(Production* production) 
{
	assert(this);
	assert(production);
	this->production = production;
}

void Opcode::addArguments(ParameterList* plist) 
{
	args.push_back(plist);
}
