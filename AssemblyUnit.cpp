// AssemblyUnit.cpp: implementation of the AssemblyUnit class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "AssemblyUnit.hpp"
#include "ProcessorDefinition.hpp"
#include "Plant.hpp"
#include "Listing.hpp"
#include "InputLine.hpp"
#include "LabelParser.hpp"
#include "KeywordParser.hpp"
#include "AssemblyParser.hpp"
#include "FreeFormatParser.hpp"
#include "Exception.hpp"
#include "Error.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AssemblyUnit::AssemblyUnit(ProcessorDefinition* proc)
: processor(proc)
{
	assert(proc != 0);

	globals = new SymbolTable();
	currentSymbols = globals;
	globals->setParent(&(proc->getSymbols()));
	line = 0;
	plant = new Plant();

	radix = 10;						
	bool relocatable = true;		
	currentScope = Symbol::_private;
	modules = new ModuleList();

	path = 0;			

	parent = 0;			

}

// AssemblyUnit for include files.  Inherits most of the settings
// from the parent assembly unit.
AssemblyUnit::AssemblyUnit(AssemblyUnit* parent)
{
	processor = parent->processor;
	globals = parent->globals;
	currentSymbols = parent->currentSymbols;
	plant = parent->plant;
	radix = parent->radix;
	relocatable = parent->relocatable;
	currentScope = parent->currentScope;
	modules = parent->modules;

	path = 0;			
	line = 0;

	this->parent = parent;			

}

// Destroys the assembly unit - if has a parent doesn't destroy its
// components as it will have inherited them from the parent.
AssemblyUnit::~AssemblyUnit()
{
	if(parent != 0)
	{
		delete globals;
		delete plant;
		delete modules;
	}	

}

/*********************************************************/
/** PROCESS_FILE does a single pass of the input file	**/
/*********************************************************/
void AssemblyUnit::process(int pass)
{
	while(line->read_line())
		{
		LabelParser labels(line);
		KeywordParser keywords(line, this, plant);
		AssemblyParser asssembly(processor, line, plant);
		FreeFormatParser freeFormat( line, plant);

		line->strip_comments(';');
		labels.parse(currentSymbols, pass, &(plant->getCurrentSegment()), currentScope);			

		for(;;)						/* cheat....*/
			{
			if (line->blank_line()) break;
			if (keywords.parse(pass)) break;
			if (keywords.parseEqu(pass)) break;
			if (asssembly.parse(pass)) break;
			if (freeFormat.parse(pass)) break;
			if (pass==1) Exception::error(SYNTAX,line);
			break;
			}
		}
	return;
}

std::string AssemblyUnit::makeName(const char* path, const char* ext)
{
	std::string name(path);
	std::string::size_type pos = name.find_last_of('.');
	if(pos != std::string::npos)
	{
		name.resize(pos);

	} 
	name.append(ext);

	return name;
}

void AssemblyUnit::process(const char* path) 
{
	assert(this);
	assert(path);

	this->path = path;

	printf("%s\n",path);
	
	line = new InputLine(path);
	if(line->isEOF())
		{
		Exception::error(NO_IP_FILE,line);
		return;
		}
	
	/* make up and open output file (suffix .AOF) */
	std::string output = makeName(path,".aof");
	plant->openOutput(output.c_str());

	/* make up and open listing file (suffix .LST) */
	std::string listing = makeName(path,".lst");
	plant->openListing(listing.c_str());

	/* Now go and crunch the file */

	int pass=1;

	printf("PASS 1\n");
	process(pass);

	line->reset_input();
	plant->resetSegments();
	pass=2;
	printf("PASS 2\n");

	plant->setPass(pass);
	plant->plant_comment(path);
	plant->plant_endian();

	line->registerListener(plant);

	process(pass);

	line->removeListener(plant);

	line->reset_input();
	plant->write_public(globals);

	globals->dump_sym_tab(stdout);
	plant->listSymbolTable(globals);
	
	delete line;
}


void AssemblyUnit::linkSymbols(SymbolTable* symbols){
	assert(this);
	assert(symbols);

	symbols->setParent(currentSymbols);
	currentSymbols = symbols;
}

void AssemblyUnit::unlinkSymbols(){
	assert(this);

	SymbolTable* symbols = currentSymbols;
	currentSymbols = currentSymbols->getParent();
	assert(currentSymbols);
	
	symbols->detachParent();
}
