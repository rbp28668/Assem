// ProcessorDefinition.hpp
//
//////////////////////////////////////////////////////////////////////

#ifndef PROCESSORDEFINITION_HPP_INCLUDED
#define PROCESSORDEFINITION_HPP_INCLUDED

#include <list>
#include <map>
#include <string>
#include "ParameterList.hpp"
#include "ProductionList.hpp"
#include "SymbolTable.hpp"

class Opcode;
class Set;	// forward
class Parameter;


class ProcessorDefinition  
{
public:
	typedef std::list<Opcode*> OpcodeListT;

private:

	typedef std::map<std::string, Set*> SetsT;
	typedef std::map<std::string, ParameterList*> ParameterListsT;

	bool	little_endian;			// true if processor stores data lsb first 
	OpcodeListT opcodes;			// opcodes for this processor

	SetsT sets;						// named sets
	ParameterListsT parameterLists;	// named parameter lists
	ProductionList productions;		// named productions.
	SymbolTable predefinedSymbols;	// such as 8051 memory mapped registers.
	
public:
	ProcessorDefinition();
	virtual ~ProcessorDefinition();

	void setLittleEndian(bool littleEndian);
	bool isLittleEndian() const;

	OpcodeListT& getOpcodes();
	void addOpcode(Opcode* opcode);

	const ProductionList& getProductions() const;
	void addProduction(Production* production, const char* name);
	
	void addSet(Set* set, const char* name);
	Set* lookupSet(const char* name);

	void addParameterList(ParameterList* plist, const char* name);
	ParameterList* lookupParameterList(const char* name);

	void addSymbol(Symbol* symbol);
	SymbolTable& getSymbols();
};

#endif 
