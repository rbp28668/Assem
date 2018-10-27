// ProductionList.hpp: interface for the ProductionList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PRODUCTIONLIST_HPP_INCLUDED
#define PRODUCTIONLIST_HPP_INCLUDED

#pragma warning(disable:4786)

#include <string>
#include <map>

class Production;

class ProductionList  
{


	struct s_prodn
		{
		Production* production;
		int index;

		};

	typedef std::map<const std::string, s_prodn*> PRODNLIST;

	PRODNLIST list;	// map of productions.
	int pindex;		// current production index;
	Production **xlat;	// translation table to get production by index

public:
	ProductionList();
	virtual ~ProductionList();

	void add(Production* prodn, const std::string& tag);
	const Production*  lookup(int idx) const;
	int indexOf(const std::string& name) const;
	void buildIndex();

};

#endif 