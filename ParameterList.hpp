// ParameterList.hpp: interface for the ParameterList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PARAMETERLIST_HPP_INCLUDED
#define PARAMETERLIST_HPP_INCLUDED

#include <list>

class Parameter;
class Production;

// List of parameters.  If all the parameters match the line being parsed
// then the attached production should be run.
class ParameterList	{
	typedef std::list<Parameter*> PListT;
	PListT parse;
	Production* production;
public:
	ParameterList();
	~ParameterList();

	const Production& getProduction() const;
	const PListT& getParameters() const;
	void addParameter(Parameter* param);
	void setProduction(Production* production);
};	

#endif 
