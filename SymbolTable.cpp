#include "SymbolTable.hpp"
#include <assert.h>
	
Symbol::Symbol() 
{
	name = "";
	scope = s_undef;
	address = 0;
	resolved = false;
}

Symbol::~Symbol()
{
}


SymbolTable::SymbolTable()
{
}

SymbolTable::~SymbolTable()
{
	for(SymbolsT::iterator iter = symbols.begin(); iter != symbols.end(); ++iter)
	{
		delete(*iter);
	}
}

void SymbolTable::setParent(SymbolTable *parent)
{
	assert(this);
	assert(parent);
	this->parent = parent;
}

SymbolTable* SymbolTable::getParent()
{
	assert(this);
	return parent;
}

void SymbolTable::detachParent()
{
	assert(this);
	parent = 0;
}

Symbol* SymbolTable::find(const std::string& name)
{
	assert(this);
	Symbol* result = 0;
	for(SymbolsT::iterator iter = symbols.begin(); iter != symbols.end(); ++iter)
	{
		Symbol* symbol = *iter;
		if(symbol->name.compare(name) == 0) 
		{
			result = symbol;
			break;
		}
	}

	if(result == 0 && parent != 0){
		result = parent->find(name);
	}

	return result;
}

bool SymbolTable::contains(const std::string& name)
{
	assert(this);
	return find(name) != 0;
}

void SymbolTable::insert(Symbol* symbol)
{
	assert(this);
	assert(symbol);
	symbols.push_back(symbol);
}

void SymbolTable::remove(const std::string& name)
{
	assert(this);
	assert(contains(name));

	for(SymbolsT::iterator iter = symbols.begin(); iter != symbols.end(); ++iter)
	{
		Symbol* symbol = *iter;
		if(symbol->name.compare(name) == 0) 
		{
			symbols.erase(iter);
			delete symbol;
			break;
		}
	}

}


static char *scope[]={"undefined","public","private","local",
			"external","equate"};

/*********************************************************/
/** DUMP_SYM_TAB dumps the symbol table to outfp	**/
/*********************************************************/
void SymbolTable::dump_sym_tab(FILE *outfp) const
{
	if (!isEmpty())
	{
		fprintf(outfp,"GLOBAL SYMBOL TABLE\n");

		for(SymbolsT::const_iterator iter = symbols.begin(); iter != symbols.end(); ++iter)
		{
			const Symbol* symbol = *iter;

			fprintf(outfp,"%s %s %08.8lx %s\n",symbol->getName(),scope[symbol->getScope()],
					symbol->getAddress(),
					(symbol->isResolved())?"":"UNRESOLVED");
		}
	}

//	mod=current->modules;
//	while(mod!=NULL)
//		{
//		fprintf(outfp,"MODULE %s\n",mod->name);
//		mod=mod->next;
//		}
	return;
}

