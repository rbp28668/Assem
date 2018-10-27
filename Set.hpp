#ifndef SET_HPP_INCLUDED
#define SET_HPP_INCLUDED

#include <string>
#include <list>

// Set is equivalent to an enumeration.  Used for creating an enumerated
// set of things such as register values.

class SetEntry {
public:
	std::string tag;
	int value;

	SetEntry(const char* name, int val) :tag(name), value(val) {}
};



class Set {

public:
	typedef std::list<SetEntry*> SetT;

private:
	SetT set;
	
	
public:
	Set();
	~Set();
	const SetEntry* find(const std::string& name) const;
	bool contains(const std::string& name) const;
	void insert(SetEntry* entry);
	void remove(const std::string& name);

	const SetT& getEntries() const {return set;}
};

#endif