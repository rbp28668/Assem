// Segment.cpp: implementation of the CSegment class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "Segment.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Segment::Segment(const std::string& _name)
: name(_name)
{
	position = 0;
}

Segment::~Segment()
{

}

long Segment::getAddress() const 
{
	assert(this);
	return position;
}

const std::string& Segment::getName() const 
{
	assert(this);
	return name;
}

void Segment::setAddress(long pos) 
{
	assert(this);
	position = pos;
}

void Segment::bump(int nBytes) 
{
	assert(this);
	assert(nBytes > 0);
	position += nBytes;
}
