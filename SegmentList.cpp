// SegmentList.cpp: implementation of the SegmentList class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786)

#include <assert.h>
#include "SegmentList.h"
#include "Segment.hpp"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SegmentList::SegmentList()
{

}

SegmentList::~SegmentList()
{
	for(SegmentsT::iterator iter = segments.begin();
	iter != segments.end();
	++iter)
	{
		delete iter->second;
	}
}

void SegmentList::addSegment(Segment* seg)
{
	assert(this);
	assert(seg);
	assert(!contains(seg->getName()));
	
	segments.insert(SegmentsT::value_type(seg->getName(), seg));
}

Segment* SegmentList::lookup(const std::string& name)
{
	assert(this);
	Segment* seg = 0;
	SegmentsT::iterator iter = segments.find(name);
	if(iter != segments.end())
	{
		seg = iter->second;
	}
	return seg;
}

bool SegmentList::contains(const std::string& name)
{
	assert(this);
	return segments.find(name) != segments.end();
}

void SegmentList::resetAddresses()
{
	assert(this);

	for(SegmentsT::iterator iter = segments.begin();
	iter != segments.end();
	++iter)
	{
		iter->second->setAddress(0);
	}
}
