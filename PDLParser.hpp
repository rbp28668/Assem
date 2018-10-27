// PDLParser.hpp: interface for the PDLParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PDLPARSER_HPP_INCLUDED
#define PDLPARSER_HPP_INCLUDED


#include "InputLine.hpp"

class ProcessorDefinition;
class Production;
class ParameterList;
class Opcode;

class PDLParser  
{
	enum prs_key {proc,msfirst,lsfirst,prs_set,opcode,arg,nul,end,plist,prodn,equ};

	enum {PBUFFLEN = 256};

	static KEYMATCH prs_keys[];
	static KEYMATCH prodn_keys[];

	InputLine* line;

	void process_set(ProcessorDefinition* processor);
	void process_equ(ProcessorDefinition* processor);
	Production* get_production(ProcessorDefinition* processor);
	ParameterList* get_opcode_arg(ProcessorDefinition* processor);
	Opcode* new_opcode(const char *name);
	void process_opcode(ProcessorDefinition* processor);

public:
	PDLParser(InputLine* input);
	virtual ~PDLParser();

	ProcessorDefinition* PDLParser::create();

	const char* lookupProduction(int prod);

};

#endif 
