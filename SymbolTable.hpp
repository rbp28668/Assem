#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <list>
#include <string>

class Segment;

class Symbol {
	
public:
	// enum asm_type {t_undef,code,data};

	enum ScopeT {s_undef,_public,_private,local,external,equate};

	std::string name;		// symbol name
	enum ScopeT	scope;		// it's scope
	Segment* segment;		// which memory segment it references (replaces type)
	long address;			// address in the segment
	bool resolved;			// true if resolved.
	bool relocatable;		// true if relocatable (replaces current->reloc)

	inline const std::string& getName() const {return name;}
	inline long getAddress() const {return address;}
	inline ScopeT getScope() const {return scope;}
	inline bool isRelocatable() const {return relocatable;}
	inline bool isResolved() const {return resolved;}
	inline const Segment& getSegment() const {return *segment;}

	inline bool isExternal() const {return scope == external;}

	inline void setName(const std::string& nm) {name = nm;}
	inline void setSegment(Segment* seg) {segment = seg;}
	inline void setAddress(long addr) {address = addr;}
	inline void setResolved(bool resolved) {this->resolved = resolved;}
	inline void setScope(ScopeT scope) {this->scope = scope;}
	Symbol();
	~Symbol();
};

	
class SymbolTable {
public:
	typedef std::list<Symbol*> SymbolsT;
	typedef SymbolsT::iterator iterator;

private:
	SymbolsT symbols;
	SymbolTable* parent;

public:
	SymbolTable();
	~SymbolTable();

	void setParent(SymbolTable *parent);
	SymbolTable* getParent();
	void detachParent();

	Symbol* find(const std::string& name);
	bool contains(const std::string& name);
	void insert(Symbol* symbol);
	void remove(const std::string& name);

	bool isEmpty() const {return symbols.empty;;}
	iterator begin() {return symbols.begin();}
	iterator end() {return symbols.end();}

	void dump_sym_tab(FILE *outfp) const;
};

#endif

