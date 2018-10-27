// AssemblyParser.cpp: implementation of the AssemblyParser class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "AssemblyParser.hpp"
#include "ProcessorDefinition.hpp"
#include "InputLine.hpp"
#include "Parameter.hpp"
#include "Set.hpp"
#include "Opcode.hpp"
#include "Exception.hpp"
#include "Error.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AssemblyParser::AssemblyParser(ProcessorDefinition* proc, InputLine* input, Plant* plant)
: processor(proc)
, line(input)
, engine(&(proc->getProductions()), plant)
{
	assert(proc != 0);
	assert(input != 0);
}

AssemblyParser::~AssemblyParser()
{

}

/*****************************************************************/
/** FIND_SET_MEMBER looks at the start of the input buffer to	**/
/** see if it matches any member of the given set. If it does	**/
/** match then TRUE is returned and *value is set to the	**/
/** value of the member.					**/
/*****************************************************************/
bool AssemblyParser::find_set_member(int& value, const Set *set)
{
	assert(this);
	assert(set);

	for(Set::SetT::const_iterator iter = set->getEntries().begin();
	iter != set->getEntries().end();
	++iter)
	{
		SetEntry* entry = *iter;
		if(line->is_sym(entry->tag.c_str()))
		{
			value = entry->value;
			return true;
		}
	}
	return false;
}


/*************************************************************************/
/** PARSE_SINGLE_ARG takes a parse list for a single argument and tries	**/
/** to parse it. If the parse is successfull the matching argument is	**/
/** taken from the input buffer and TRUE is returned. If the argument	**/
/** does not match then the input buffer is left unchanged and FALSE	**/
/** is returned.							**/
/*************************************************************************/
bool AssemblyParser::parse_single_arg(const ParameterList& arg)
{
	assert(this);

	char* mark = line->getMark();

	for(ParameterList::PListT::const_iterator iter = arg.getParameters().begin();
	iter != arg.getParameters().end();
	++iter)
	{

		const Parameter* param = *iter;

		switch(param->getType())
			{
			// Text - need to match literal text
			case Parameter::text_type:
				if(!line->is_sym(param->getItem()))
					{
					line->revertToMark(mark);
					return false;
					}
				break;

			// Set - need to match any set member.
			case Parameter::set_type:
				{
				int value = 0;
				if(!find_set_member(value,param->getItemAsSet()))
					{
					line->revertToMark(mark);
					return false;
					}
				else
					engine.push(Expression::ito_expr(value));
				break;
				}

			case Parameter::expr_type:
				{
				Expression* ex = line->get_expression();
				if(ex == 0)
					{
					line->revertToMark(mark);
					return false;
					}
				else
					engine.push(ex);
				break;
				}

			case Parameter::plist_type:
				if (!parse_single_arg(*(param->getItemAsPList())))
					return false;
				break;

			default:
				Exception::fatal("parse type");
				break;
			}
		}
	return true;
}

/*************************************************************************/
/** PARSE_ASM_ARGS takes an argument list for a given opcode and tries	**/
/** to match one of the possible sets of arguments. If a match is found	**/
/** the corresponding production rules are run and TRUE is returned. 	**/
/** if none of the arguments match then FALSE is returned		**/
/*************************************************************************/
bool AssemblyParser::parse_asm_args(const Opcode& opcode)
{
	assert(this);
	
	for(Opcode::PListListT::const_iterator args = opcode.getArgs().begin();
	args != opcode.getArgs().end();
	++args)
	{
		ParameterList* params = *args;		
		engine.clearStack();
		if(parse_single_arg(*params)) // matches this set of params?
		{
			engine.exe_prodn(params->getProduction());
			return true;
		}
	}
	return false;
}

/*************************************************************************/
/** ASM_PARSE tries to match an assembler opcode with is optional 	**/
/** arguments. If it finds a matching opcode then, if there are no	**/
/** arguments the production rules are run, otherwise it tries to match	**/
/** one of its possible sets of arguments.				**/
/*************************************************************************/
bool AssemblyParser::parse(int pass)
{
	assert(this);

	for(ProcessorDefinition::OpcodeListT::const_iterator iter = processor->getOpcodes().begin(); 
	iter != processor->getOpcodes().end();
	++iter)
	{
		Opcode* opcode = *iter;
		if(line->is_sym(opcode->getName().c_str()))
			{
			if(opcode->hasArgs())
				{
				if(!parse_asm_args(*opcode))
					if(pass==2) 
						Exception::error(SYNTAX, line);
				}
			else
				engine.exe_prodn(opcode->getProduction());
			return true;
			}
		}
	return true;
}



