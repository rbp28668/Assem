// AssemblyUnit.hpp: interface for the AssemblyUnit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ASSEMBLYUNIT_HPP_INCLUDED
#define ASSEMBLYUNIT_HPP_INCLUDED

#include "SymbolTable.hpp"
#include "ModuleList.hpp"

class ProcessorDefinition;
class InputLine;
class Plant;

class AssemblyUnit  
{
	ProcessorDefinition* processor;
	SymbolTable* globals;
	SymbolTable* currentSymbols;	// current table to use for lookups.
	InputLine* line;
	Plant* plant;

	int radix;						// current radix for numbers
	bool relocatable;				// true if current complilation is relocatable // maybe should be in Segment
	Symbol::ScopeT currentScope;	// current scope - local, global etc.
	ModuleList* modules;

	const char* path;				// current input file.

	AssemblyUnit* parent;			// parent file if this is include.


//?	void execute_keyword(KEYWORD key, int pass);

	void process(int pass);
	std::string makeName(const char* path, const char* ext);

public:
	AssemblyUnit(AssemblyUnit* parent);
	AssemblyUnit(ProcessorDefinition* proc);
	
	virtual ~AssemblyUnit();

	void process(const char* path);

	SymbolTable& getGlobals() { return *globals;}
	ModuleList& getModules() {return *modules;}

	void setRadix(int radix) {this->radix = radix;}

	void setRelocatable(bool relocatable) {this->relocatable = relocatable;}
	bool isRelocatable() {return relocatable;}
	
	void setScope(Symbol::ScopeT scope) {currentScope = scope;}
	Symbol::ScopeT getScope() {return currentScope;}

	void linkSymbols(SymbolTable* symbols);
	void unlinkSymbols();
};

#endif 
