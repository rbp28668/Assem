// LabelParser.hpp: interface for the LabelParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LABELPARSER_HPP_INCLUDED
#define LABELPARSER_HPP_INCLUDED

#include "SymbolTable.hpp"

class InputLine;
class Segment;

class LabelParser  
{
	InputLine* line; 

	char *skip();
public:
	LabelParser(InputLine* input);
	virtual ~LabelParser();

	void parse(SymbolTable* symtab, int pass, Segment* currentSegment, Symbol::ScopeT scope);
};

#endif 
