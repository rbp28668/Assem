// ParameterList.cpp: implementation of the ParameterList class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "ParameterList.hpp"
#include "Parameter.hpp"
#include "Production.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParameterList::ParameterList()
: production(0)
{

}

ParameterList::~ParameterList()
{
	for(PListT::iterator iter = parse.begin();
	iter != parse.end();
	++iter)
	{
		Parameter* param = *iter;
		delete param;
	}

	delete production;
}

const Production& ParameterList::getProduction() const 
{
	assert(this);
	assert(production);
	return *production;
}

const ParameterList::PListT& ParameterList::getParameters() const 
{
	assert(this);
	return parse;
}

void ParameterList::addParameter(Parameter* param) 
{
	assert(this);
	assert(param);
	parse.push_back(param);
}

void ParameterList::setProduction(Production* production) 
{
	assert(this);
	assert(production);
	this->production = production;
}
