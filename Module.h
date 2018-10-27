// Module.h: interface for the CModule class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include <string>
#include "SymbolTable.hpp"

class Module  
{
	SymbolTable symbolTable;

public:
	Module();
	~Module();

	SymbolTable& getSymbols() { return symbolTable;}
};

#endif
