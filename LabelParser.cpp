// LabelParser.cpp: implementation of the LabelParser class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "LabelParser.hpp"
#include "Error.h"
#include "Exception.hpp"
#include "InputLine.hpp"
#include "Segment.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LabelParser::LabelParser(InputLine* input)
: line(input)
{
	assert(input);
}

LabelParser::~LabelParser()
{

}



/*****************************************************************/
/** PROCESS_LABEL skips over the label in the input buffer and	**/
/** if a valid one is found then it is stuffed into the 	**/
/** appropriate symbol table					**/
/*****************************************************************/
void LabelParser::parse(SymbolTable* symtab, int pass, Segment* currentSegment, Symbol::ScopeT scope)
{
	assert(this);
	assert(symtab);

	line->skip_space();

	char* name = line->skipLabel();
	if(name == 0)
		return;

	if(pass==1)
	{
		std::string label(name);
		if(symtab->contains(label))
		{
			Exception::error(MULT_LOCAL_DEF, line);
			//Exception::error(MULT_GLOBAL_DEF);
		} 
		else
		{
			Symbol* symbol = new Symbol();
			symbol->setName(label);
			symbol->setSegment(currentSegment);
			symbol->setAddress(currentSegment->getAddress());
			symbol->setScope(scope);
			symbol->setResolved(true);

			symtab->insert(symbol);
		}
	}
		
	return;
}
