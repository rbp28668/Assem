// Parameter.cpp: implementation of the Parameter class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "Parameter.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Parameter::Parameter()
{
	type = expr_type;
	item = 0;
}

Parameter::Parameter(const char* text)
{
	assert(text);
	type = text_type;
	item = text;
}

Parameter::Parameter(const Set* set)
{
	assert(set);
	type = set_type;
	item = set;
}

Parameter::Parameter(const ParameterList* plist)
{
	assert(plist);
	type = plist_type;
	item = plist;
}


Parameter::~Parameter()
{

}

Parameter::Type Parameter::getType() const 
{
	assert(this);
	return type;
}

const char* Parameter::getItem() const 
{
	assert(this);
	assert(type == text_type);
	return static_cast<const char *>(item);
}

const Set* Parameter::getItemAsSet() const 
{
	assert(this);
	assert(type == set_type);
	return static_cast<const Set*>(item);
}

const ParameterList* Parameter::getItemAsPList() const 
{
	assert(this);
	assert(type == plist_type);
	return static_cast<const ParameterList*>(item);
}
