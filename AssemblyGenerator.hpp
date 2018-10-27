// AssemblyGenerator.hpp: interface for the AssemblyGenerator class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ASSEMBLYGENERATOR_HPP_INCLUDED
#define ASSEMBLYGENERATOR_HPP_INCLUDED

#include <string>

#include "ProcessorDefinition.hpp"
#include "ParameterList.hpp"

class AssemblyGenerator  
{
private:
	void write_assem(std::string& fixed, 
	const ParameterList* params, ParameterList::PListT::const_iterator iter,
	const ParameterList* rest, ParameterList::PListT::const_iterator iterRest
	);

	void write_opcodes(ProcessorDefinition* processor);
public:
	AssemblyGenerator();
	virtual ~AssemblyGenerator();

	void write(ProcessorDefinition* processor);

};

#endif 