// Parameter.hpp: interface for the Parameter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PARAMETER_HPP_INCLUDED
#define PARAMETER_HPP_INCLUDED

class Set;
class ParameterList;

// A parameter to an opcode
class Parameter	{
public:
	enum Type {
		text_type,	// Literal text.
		set_type,	// One of a number of definitions from a set
		expr_type,  // a numeric expression.
		plist_type	// a shared parameter list.
	};

private:
	Type type;
	const void *item;

public:
	Parameter();
	Parameter(const char* text);
	Parameter(const Set* set);
	Parameter(const ParameterList* plist);
	~Parameter();

	Type getType() const;
	const char* getItem() const;
	const Set* getItemAsSet() const;
	const ParameterList* getItemAsPList() const;
};

#endif
