// PDLParser.cpp: implementation of the PDLParser class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)

#include <assert.h>
#include "PDLParser.hpp"
#include "ProcessorDefinition.hpp"
#include "Production.hpp"
#include "Exception.hpp"
#include "SymbolTable.hpp"
#include "Parameter.hpp"
#include "Set.hpp"
#include "Opcode.hpp"
#include "Error.h"

KEYMATCH PDLParser::prs_keys[]={
		"arg",arg,
		"opcode",opcode,
		"nul",nul,
		"end",end,
		"set",prs_set,
		"plist",plist,
		"prodn",prodn,
		"proc",proc,
		"msfirst",msfirst,
		"lsfirst",lsfirst,
		"equ",equ,
		NULL,-1};


KEYMATCH PDLParser::prodn_keys[]={
	"dup",	Production::dupl,
	"swap",	Production::swap,
	"over",	Production::over,
	"pick",	Production::pick,
	"drop",	Production::drop,
	"+",	Production::plus,
	"-",	Production::minus,
	">>",	Production::shr,
	"<<",	Production::shl,
	"~",	Production::not,
	"neg",	Production::neg,
	"^",	Production::xor,
	"&",	Production::and,
	"|",	Production::or,
	"rel8",	Production::rel8,
	"rel16",Production::rel16,
	"abs16",Production::abs16,
	"byte",	Production::byte,
	"word",	Production::word,
	";",	Production::pend,
	NULL,-1};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PDLParser::PDLParser(InputLine* input)
	: line(input)
{
	assert(input);
	
}

PDLParser::~PDLParser()
{
	
}
	

/*****************************************************************/	
/** PROCESS_SET reads in and places in the set list  a set 	**/
/** description 						**/
/*****************************************************************/	
void PDLParser::process_set(ProcessorDefinition* processor)
{
	
	char* name = line->get_name();
	if(name == 0)
		Exception::error(NO_SET_NAME, line);
	
	if(!line->is_sym("{"))
		Exception::error(MISSING_L_BRACKET, line);
	
	Set* set = new Set();
	
	int value=0;
	do
	{
		char* item = line->get_name();
		if(line->is_sym("=")) value = line->get_int();
		
		SetEntry* entry = new SetEntry(item, value);
		set->insert(entry);
		
		delete [] item;
		++value;
	}
	while(line->is_sym(","));
	
	if(!line->is_sym("}")) 
		Exception::error(MISSING_R_BRACKET, line);
	
	processor->addSet(set,name);
	
	delete [] name;
	return;
}

/*****************************************************************
* process_equ
* Processes an equate to allow symbols to be set up in the 
* processor symbol table.  This is for processors like the 8051 
* which have a number of memory or io mapped peripherals at 
* named addresses.
*****************************************************************/	
void PDLParser::process_equ(ProcessorDefinition* processor)
{
	assert(this);
	assert(processor);
	
	char* name = line->get_name();
	if(name == 0)
	{
		Exception::error(NO_PRODN_NAME, line); // TBD
	}
	else
	{
		int value = line->get_int();
		Symbol* symbol = new Symbol();
		symbol->setName(name);
		symbol->setAddress(value);
		symbol->setResolved(true);
		symbol->setScope(Symbol::equate);

		processor->addSymbol(symbol);
		delete [] name;
	}
}

/*****************************************************************/
/** GET_PRODUCTION reads in and returns a series of production	**/
/** rules in RPN						**/	
/*****************************************************************/	
Production* PDLParser::get_production(ProcessorDefinition* processor)
{
	int prodbuff[PBUFFLEN];
	
	int pbuffptr=0;
	int key = -1;
	do
	{
		line->check_line();
		line->skip_space();
		if(isdigit(line->current()))	/* an integer ? */
		{
			int value = line->get_int();
			prodbuff[pbuffptr++] = Production::literal;
			prodbuff[pbuffptr++] = value;
			key=-1;			/* anything but pend ! */
		}
		else
		{
			key = line->get_keyword(prodn_keys);
			if(key==-1)			/* keyword not found ? */
			{
				char* name = line->get_name();	/* production name ? */
				int idx = processor->getProductions().indexOf(name);
				if(idx==-1)
				{				
					Exception::error(PRODN_NOT_FOUND, line);
					line->read_line();	/* give up on this one ! */
					key = Production::pend;				
				}
				prodbuff[pbuffptr++] = Production::defn;
				prodbuff[pbuffptr++] = idx;
				delete [] name;
			}
			else
				prodbuff[pbuffptr++] = key;
		}
	}
	while(key!=Production::pend);
	
	Production* prod = new Production(prodbuff, pbuffptr);
	
	return(prod);
}

/*****************************************************************/
/** GET_OPCODE_ARG reads in and returns in a list, an argument 	**/
/** for an opcode. This is returned as a plist that can then	**/
/** be linked	into the opcodes argument list.			**/
/*****************************************************************/	
ParameterList* PDLParser::get_opcode_arg(ProcessorDefinition* processor)
{
	
	ParameterList* parameterList = new ParameterList();
	
	do
	{
		Parameter* param = 0;
		
		if(line->is_sym("#"))			/* expression ? */
		{
			param = new Parameter();
			parameterList->addParameter(param);			
			continue;
		}
		
		if(line->current()=='\'')	/* string? */
		{
			param = new Parameter(line->get_string());
			parameterList->addParameter(param);			
			continue;
		}
		
		char* name = line->get_name();
		if(name != 0)	/* a set or plist ? */
		{
			Set* set = processor->lookupSet(name);
			if(set != 0)
			{
				param = new Parameter(set);
				parameterList->addParameter(param);
			} 
			else
			{
				ParameterList* plist = processor->lookupParameterList(name);
				if(plist != 0)
				{
					param = new Parameter(plist);
					parameterList->addParameter(param);
				}
				else
				{
					Exception::error(ITEM_NAME_DUFF, line);
				}
			}
			delete [] name;
		}
		else
			Exception::error(SYNTAX, line);
		
	}
	while(line->is_sym(","));
	return parameterList;
}


/*****************************************************************/
/** PROCESS_OPCODE reads in an opcode name along with and	**/
/** arguments and production rules				**/	
/*****************************************************************/	
void PDLParser::process_opcode(ProcessorDefinition* processor)
{
	assert(this);
	assert(processor);
	
	char* name = line->get_string();
	Opcode* opcode = new Opcode(name);
	delete [] name;
	
	// reset_arglist();
	
	int key;
	Production* production = 0;
	
	do
	{
		line->check_line();
		key = line->get_keyword(prs_keys);
		switch(key)
		{
		case nul:
			if(!line->is_sym("->"))
			{
				Exception::error(MISSING_PRODN, line);
				line->read_line();
				break;
			}
			production = get_production(processor);
			opcode->setProduction(production);
			break;
			
		case arg:
			{
				ParameterList* plist = get_opcode_arg(processor);
				if(!line->is_sym("->"))
				{
					Exception::error(MISSING_PRODN, line);
					line->read_line();	/* skip line with error */
					break;
				}
				production = get_production(processor);
				plist->setProduction(production);
				opcode->addArguments(plist);
				break;
			}
		case end:
			break;
		default:
			Exception::error(KEYWORD_SEQUENCE, line);
			break;
		}
	}
	while(key==arg);

	processor->addOpcode(opcode);

	return;
}

/*****************************************************************/
/** READ_PROCESSOR_DESCRIPTION reads in and sets up a complete	**/
/** processor description. The opcodes/ sets and arguments are	**/
/** then all accessed from the variable OPLIST *opcode_list	**/	
/*****************************************************************/	
ProcessorDefinition* PDLParser::create()
{
	char *name;
	
	ProcessorDefinition* procDef = new ProcessorDefinition();
	
	while(line->read_line())
	{
		line->strip_comments('/');
		if(line->blank_line()) continue;
		
		int key = line->get_keyword(prs_keys);
		switch(key)
		{
		case proc:
			name = line->get_string();
			printf("Processor %s\n",name);
			delete [] name;
			break;
			
		case msfirst:
			procDef->setLittleEndian(false);
			break;
			
		case lsfirst:
			procDef->setLittleEndian(true);
			break;
			
		case prs_set:
			process_set(procDef);
			break;
			
		case opcode:
			process_opcode(procDef);
			break;
			
		case plist:
			{
				name = line->get_name();
				if(name == 0)
				{
					Exception::error(NO_PLIST_NAME, line);
					break;
				}
				
				ParameterList* plist = get_opcode_arg(procDef);
				procDef->addParameterList(plist, name);
				delete [] name;
				break;
			}

		case prodn:
			{
				name = line->get_name();
				if(name == 0)
				{
					Exception::error(NO_PRODN_NAME, line);
					break;
				}
				
				Production* production = get_production(procDef);
				procDef->addProduction(production,name);
				delete [] name;
				break;
			}

		case equ:
			process_equ(procDef);
			break;

		case arg:
		case nul:
		case end:
			Exception::error(KEYWORD_SEQUENCE, line);
			break;
		case -1:
			Exception::error(ILLEGAL_KEYWORD, line);
			break;
		default:
			Exception::fatal("Illegal proc defn keyword");
			break;
		}
	}
	return procDef;
}

const char* PDLParser::lookupProduction(int prod){
	assert(this);
	
	for(int i=0;prodn_keys[i].name!=0;++i)
		if(prodn_keys[i].key == prod)
		{
			return prodn_keys[i].name;
		}
		return("Illegal token");
}

