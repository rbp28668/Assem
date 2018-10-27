// IntelHex.h: interface for the CIntelHex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTELHEX_H__243C6B24_F499_11D4_AF75_002018BA082E__INCLUDED_)
#define AFX_INTELHEX_H__243C6B24_F499_11D4_AF75_002018BA082E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ByteStream.h"

class CIntelHex  
{
public:
	typedef unsigned char BYTE;
	typedef unsigned short WORD;

	CIntelHex();
	virtual ~CIntelHex();
	
	void setStream(IByteStream* stream);
	
	// Writing
	void setAddress(int val);
	void write(BYTE value);
	void terminate();

	// Reading
	bool readRecord();
	WORD address() const;
	const BYTE* data() const;
	int dataLength() const;

private:
	static const char* digits;


	void resetCheckSum();
	BYTE checkSum() const;
	void phex2(BYTE value);
	void phex4(WORD value);
	void flush();

	BYTE inhex2();
	WORD inhex4();

	enum { RECSIZE = 16 }; // static const int.....
	WORD checksum;
	WORD addr;
	int nbytes;
	BYTE hex_data[RECSIZE];
	IByteStream* m_stream;

};

#endif // !defined(AFX_INTELHEX_H__243C6B24_F499_11D4_AF75_002018BA082E__INCLUDED_)
