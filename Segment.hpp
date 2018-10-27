// Segment.h: interface for the Segment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SEGMENT_H_INCLUDED_)
#define AFX_SEGMENT_H_INCLUDED_

#include <string>

// Class to create a memory segment such as a code or data segment.
class Segment  
{
	std::string name;	// name of memory segment
	long position;		// current address

public:
	Segment(const std::string& name);
	~Segment();

	long getAddress() const;
	const std::string& getName() const;

	void setAddress(long pos);
	void bump(int nBytes);

};

#endif 