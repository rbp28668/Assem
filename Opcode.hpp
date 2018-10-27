// Opcode.hpp: interface for the Opcode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef OPCODE_HPP_INCLUDED
#define OPCODE_HPP_INCLUDED

#include <list>

class ParameterList;
class Production;

// An opcode.  Each opcode has a list of possible argument lists and a production.
// The production is for when the opcode has a no argument version.
class Opcode {
	std::string opcode;	
	typedef std::list<ParameterList*> PListListT;
	PListListT args;
	Production* production;

public:
	Opcode(char* name);
	~Opcode();

	const std::string& getName() const;
	const bool hasArgs() const;
	const PListListT& getArgs() const;
	const Production& getProduction() const;
	void setProduction(Production* production);
	void addArguments(ParameterList* plist);
};


#endif
