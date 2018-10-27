// AssemblyParser.hpp: interface for the AssemblyParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ASSEMBLYPARSER_HPP_INCLUDED
#define ASSEMBLYPARSER_HPP_INCLUDED

#include "Production.hpp"

class InputLine;
class ProcessorDefinition;
class Opcode;
class ParameterList;
class Set;
class Plant;

class AssemblyParser  
{
	InputLine* line; 
	ProcessorDefinition* processor;
	ProductionEngine engine;

	bool find_set_member(int& value, const Set *set);
	bool parse_asm_args(const Opcode& opcode);
	bool parse_single_arg(const ParameterList& arg);
public:
	AssemblyParser(ProcessorDefinition* proc, InputLine* input, Plant* plant);
	virtual ~AssemblyParser();

	bool parse(int pass);
};

#endif 