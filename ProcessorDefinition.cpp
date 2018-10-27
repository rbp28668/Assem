// ProcessorDefinition.cpp: implementation of the ProcessorDefinition class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable: 4786)

#include <assert.h>
#include "ProcessorDefinition.hpp"
#include "SymbolTable.hpp"
#include "Opcode.hpp"
#include "Set.hpp"
#include "ParameterList.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ProcessorDefinition::ProcessorDefinition()
{

}

ProcessorDefinition::~ProcessorDefinition()
{
	for(OpcodeListT::iterator iterOp = opcodes.begin();
	iterOp != opcodes.end();
	++iterOp)
	{
		Opcode* opcode = *iterOp;
		delete opcode;
	}

	for(SetsT::iterator iter = sets.begin();
	iter != sets.end();
	++iter)
	{
		Set* set = iter->second;
		delete set;
	}

	for(ParameterListsT::iterator iterPL = parameterLists.begin();
	iterPL != parameterLists.end();
	++iterPL)
	{
		ParameterList* pl = iterPL->second;
		delete pl;
	}

}

void ProcessorDefinition::addSet(Set* set, const char* name)
{
	assert(this);
	assert(set);
	assert(name);

	sets.insert(SetsT::value_type(std::string(name), set));
}

Set* ProcessorDefinition::lookupSet(const char* name)
{
	assert(this);
	assert(name);
	SetsT::iterator iter = sets.find(name);
	if(iter != sets.end())
		return iter->second;
	else
		return 0;
}

void ProcessorDefinition::addParameterList(ParameterList* plist, const char* name)
{
	assert(this);
	assert(plist);
	assert(name);

	parameterLists.insert(ParameterListsT::value_type(name,plist));
}

ParameterList* ProcessorDefinition::lookupParameterList(const char* name)
{
	assert(this);
	assert(name);

	ParameterListsT::iterator iter = parameterLists.find(name);
	if(iter != parameterLists.end())
		return iter->second;
	else
		return 0;
}



void ProcessorDefinition::setLittleEndian(bool littleEndian) 
{
	assert(this);
	little_endian = littleEndian;
}

bool ProcessorDefinition::isLittleEndian() const 
{
	assert(this);
	return little_endian;
}

void ProcessorDefinition::addSymbol(Symbol* symbol)
{
	assert(this);
	assert(symbol);
	predefinedSymbols.insert(symbol);
}

SymbolTable& ProcessorDefinition::getSymbols()
{
	return predefinedSymbols;
}

ProcessorDefinition::OpcodeListT& ProcessorDefinition::getOpcodes() 
{
	assert(this);
	return opcodes;
}

void ProcessorDefinition::addOpcode(Opcode* opcode) 
{
	assert(this);
	assert(opcode);
	opcodes.push_back(opcode);
}

const ProductionList& ProcessorDefinition::getProductions() const 
{
	assert(this);
	return productions;
}

void ProcessorDefinition::addProduction(Production* production, const char* name) 
{
	assert(this);
	assert(production);
	assert(name);
	productions.add(production, name);
}
