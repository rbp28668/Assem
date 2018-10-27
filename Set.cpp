#include <assert.h>
#include "Set.hpp"

Set::Set()
{
}

Set::~Set()
{
	for(SetT::iterator iter = set.begin(); iter != set.end(); ++iter)
	{
		delete(*iter);
	}
}

const SetEntry* Set::find(const std::string& tag) const
{
	assert(this);

	SetEntry* result = 0;
	for(SetT::const_iterator iter = set.begin(); iter != set.end(); ++iter)
	{
		SetEntry* entry = *iter;
		if(entry->tag.compare(tag) == 0) 
		{
			result = entry;
			break;
		}
	}
	return result;
}

bool Set::contains(const std::string& tag) const
{
	assert(this);
	return find(tag) != 0;
}

void Set::insert(SetEntry* entry)
{
	assert(this);
	assert(entry);

	set.push_back(entry);
}

void Set::remove(const std::string& tag)
{
	assert(this);
	assert(contains(tag));

	for(SetT::iterator iter = set.begin(); iter != set.end(); ++iter)
	{
		SetEntry* entry = *iter;
		if(entry->tag.compare(tag) == 0) 
		{
			set.erase(iter);
			delete entry;
			break;
		}
	}

}

