// SegmentList.h: interface for the SegmentList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTLIST_H__0DE49496_E62D_4AE4_BA9E_F817DF2E9E15__INCLUDED_)
#define AFX_SEGMENTLIST_H__0DE49496_E62D_4AE4_BA9E_F817DF2E9E15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>

class Segment;

class SegmentList  
{
public:
	typedef std::map<std::string, Segment*> SegmentsT;

private:
	SegmentsT segments;

public:
	SegmentList();
	virtual ~SegmentList();

	void addSegment(Segment* seg);
	Segment* lookup(const std::string& name);
	bool contains(const std::string& name);

	void resetAddresses();

};

#endif // !defined(AFX_SEGMENTLIST_H__0DE49496_E62D_4AE4_BA9E_F817DF2E9E15__INCLUDED_)
