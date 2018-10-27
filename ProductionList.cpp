// ProductionList.cpp: implementation of the ProductionList class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>


#include "ProductionList.hpp"
#include "Exception.hpp"
#include "error.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProductionList::ProductionList()
{
	pindex=0;
	xlat = 0;
}

ProductionList::~ProductionList()
{
	delete [] xlat;

	for(PRODNLIST::iterator entry = list.begin();
	entry != list.end();
	++entry)
	{
		delete entry->second;
	}

}


	

/*****************************************************************/
/** LINK_PRODN stuffs a prodn into the prodn store with a tag 	**/
/** given by name.						**/
/*****************************************************************/

void ProductionList::add(Production* prodn, const std::string& name)
{
	if(list.find(name) != list.end())
		Exception::error(DUPLICATE_PRODN);

	s_prodn* entry = new s_prodn();
	entry->production = prodn;
	entry->index = pindex++;

	list.insert(PRODNLIST::value_type(name,entry));
}

/*****************************************************************/
/** GET_PRODN looks for the prodn that has an index that matches**/
/** idx and returns a pointer to it if found. If not found	**/
/** get_prodn returns NULL.					**/
/*****************************************************************/

const Production* ProductionList::lookup(int idx) const
{
	if(xlat != 0)			/* translation table built? */
		return(xlat[idx]);

	for(PRODNLIST::const_iterator iter = list.begin();
	iter != list.end();
	++iter)
	{
		s_prodn* entry = iter->second;
		if(entry->index == idx)
			return entry->production;
	}


	Exception::error(PRODN_NOT_FOUND);
	return 0;
}

/*****************************************************************/
/* GET_PRODN_INDEX takes a name of a production list and 	**/
/** returns its index if found, otherwise it returns -1		**/
/*****************************************************************/
int ProductionList::indexOf(const std::string& name) const
{
	PRODNLIST::const_iterator entry = list.find(name);
	if(entry == list.end())
		return(-1);
	
	return entry->second->index;
}

/*****************************************************************/
/** BUILD_PRODUCTION_LIST runs through the list and sets up an	**/
/** array that directly translates indices into production 	**/
/** lists. The program will work without this but it will be 	**/
/** slower when it runs production code.			**/
/*****************************************************************/
void ProductionList::buildIndex()
{
	if (pindex==0) return;
	
	if(xlat != 0)
		delete [] xlat;

	xlat = new Production*[pindex];

	for(PRODNLIST::iterator entry = list.begin();
	entry != list.end();
	++entry)
	{
		xlat[entry->second->index] = entry->second->production;
	}

	return;
}
