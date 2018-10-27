// AssemblyGenerator.cpp: implementation of the AssemblyGenerator class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786)
#include <assert.h>
#include "AssemblyGenerator.hpp"
#include "Parameter.hpp"
#include "Set.hpp"
#include "Opcode.hpp"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AssemblyGenerator::AssemblyGenerator()
{

}

AssemblyGenerator::~AssemblyGenerator()
{

}

void AssemblyGenerator::write_assem(std::string& fixed, 
	const ParameterList* params, ParameterList::PListT::const_iterator iter,
	const ParameterList* rest, ParameterList::PListT::const_iterator iterRest
	)
{
	assert(this);
	assert(params);

	// Drop back at end of recursion.
	if(iter == params->getParameters().end() && rest != 0)
	{
		params = rest;
		iter = iterRest;
	}

	// end of the parameters.
	if(iter == params->getParameters().end())
	{
		puts(fixed.c_str());
		putchar('\n');
		return;
	}

	Parameter* param = *iter++;
	switch(param->getType())
	{
	case Parameter::text_type:
		{
			std::string text(fixed);
			text.append(param->getItem());
			write_assem(text, params, iter, 0 , iter);
			break;
		}

	case Parameter::set_type:
		{
			for(Set::SetT::const_iterator iterSet = param->getItemAsSet()->getEntries().begin();
			iterSet != param->getItemAsSet()->getEntries().end();
			++iterSet)
			{
				SetEntry* entry = *iterSet;
				std::string text(fixed);
				text.append(entry->tag);
				write_assem(text, params, iter, 0 , iter);
			}
			break;
		}

	case Parameter::expr_type:
		{
			std::string text(fixed);
			text.append("55");
			write_assem(text, params, iter, 0 , iter);
			break;
		}

	case Parameter::plist_type:
		{
			const ParameterList* subList = param->getItemAsPList();
			ParameterList::PListT::const_iterator iterSub = subList->getParameters().begin();
			write_assem(fixed,subList, iterSub, params, iter);
			break;
		}
	default:
		puts("Funny type");
		exit(1);
		break;
	}
return;
}

void AssemblyGenerator::write_opcodes(ProcessorDefinition* processor)
{
	assert(this);
	assert(processor);
	
	ProcessorDefinition::OpcodeListT& opcodes = processor->getOpcodes();
	
	for(ProcessorDefinition::OpcodeListT::const_iterator iter = opcodes.begin();
	iter != opcodes.end();
	++iter)
	{
		Opcode* opcode = *iter;
		
		if(!opcode->hasArgs())
			puts(opcode->getName().c_str());	/* just write the opcode */
		else
		{
			std::string text = opcode->getName();
			text.append(" ");
			
			for(Opcode::PListListT::const_iterator iter = opcode->getArgs().begin(); 
			iter != opcode->getArgs().end();
			++iter)
			{
				ParameterList* params = *iter;
				ParameterList::PListT::const_iterator iterParam = params->getParameters().begin();
				write_assem(text, params, iterParam, 0, iterParam);
			}
			
		}
	}
	return;
}

void AssemblyGenerator::write(ProcessorDefinition* processor)
{
	assert(this);
	assert(processor);
	
	write_opcodes(processor);
}

