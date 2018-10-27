// IntelHex.cpp: implementation of the CIntelHex class.
//
//////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include "IntelHex.h"
#include "Exception.hpp"

const char* CIntelHex::digits = "0123456789ABCDEF";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntelHex::CIntelHex()
: m_stream(0)
, checksum(0)
, addr(0)
, nbytes(0)
{

}

CIntelHex::~CIntelHex()
{

}

/*****************************************************************/
/** CIntelHex::resetCheckSum resets the check sum for each line of 	**/
/** intelhex							**/
/*****************************************************************/
void CIntelHex::resetCheckSum()
{
	checksum=0;
}


/*****************************************************************/
/** CIntelHex::checkSum gets the checksum for the current line	**/
/*****************************************************************/
unsigned char CIntelHex::checkSum() const
{
	return( ((unsigned char)((~checksum)+1)&0xFF));	/* form 2s complement of lsbyte */
}

/*****************************************************************/
/** CIntelHex::phex2 prints a byte as 2 hex digits				**/
/*****************************************************************/
void CIntelHex::phex2(unsigned char value)
{
	m_stream->write( digits[(value&0xF0)>>4] );
	m_stream->write( digits[(value&0x0F)]);
	checksum+=value;
}


/*****************************************************************/
/** CIntelHex::phex4 prints an integer as 4 hex digits 			**/
/*****************************************************************/
void CIntelHex::phex4(unsigned short value)
{
	phex2(unsigned char( (value & unsigned short(0xFF00)) >> 8));
	phex2(unsigned char( (value & unsigned short(0x00FF)) ));
}


/*****************************************************************/
/** CIntelHex::flush writes out the current buffer to the file	**/
/** in intel hex format						**/
/*****************************************************************/
void CIntelHex::flush()
{
	if(nbytes > 0)
	{
		resetCheckSum();
		m_stream->write(':');		/* record header */
		phex2(unsigned char(nbytes));	/* number of hex_data bytes */
		phex4(addr);		/* load address */
		phex2(0);			/* record type is hex_data */
		for(int i=0; i<nbytes; ++i)		/* write hex_data bytes */
			{
			phex2(hex_data[i]);
			++addr;
			}
		phex2(checkSum());	/* write checksum */
		m_stream->write('\n');
		nbytes=0;
	}
	return;
}

/*****************************************************************/
/** CIntelHex::terminate writes a terminating record 
/*****************************************************************/
void CIntelHex::terminate(void)
{
	flush();
	m_stream->write(":00000001FF");		/* end of file record */
	m_stream->write('\n');
}


/*****************************************************************/
/** CIntelHex::write buffers a single byte to be written to the 	**/
/** current intel_hex file. If the buffer is already full it 	**/
/** is flushed.							**/
/*****************************************************************/
void CIntelHex::write(unsigned char value)
{
	if(nbytes==RECSIZE) 
		flush();

	hex_data[nbytes++] = value;
}

/*****************************************************************/
/** CIntelHex::setAddress sets the address to which any bytes 	**/
/** following should be loaded to				**/
/*****************************************************************/
void CIntelHex::setAddress(int val)
{
	flush();
	addr=val;
}


/*****************************************************************/
/** CIntelHex::setStream sets up the file that intel hex output 	**/
/** should go to.						**/
/*****************************************************************/
void CIntelHex::setStream(IByteStream* stream)
{
	flush();
	m_stream = stream;
}

bool CIntelHex::readRecord()
{
	nbytes = 0;
	addr = 0;
	checksum = 0;

	char ch;
	while(isspace(ch = m_stream->read()))
		;

	if(ch != ':')
		Exception::fatal("Intel-hex record missing start character- ");

	nbytes = inhex2();
	addr = inhex4();
	bool isHexData = (inhex2() == 0); // otherwise terminating record

	for(int i=0; i<nbytes; ++i)
		hex_data[i] = inhex2();

	inhex2(); // checksum

	if(checksum != 0)
		Exception::fatal("Bad checksum reading Intel-hex");

	return isHexData;
}

unsigned char CIntelHex::inhex2()
{
	unsigned char val = 0;
	for(int iDigit = 0; iDigit < 2; ++iDigit)
	{
		val *= 16;

		char ch = toupper(m_stream->read());
		
		for(int i=0; i<16; ++i)
			if(ch == digits[i])
				break;

		if(i == 16)
			Exception::fatal("Bad hex digit in Intel-hex");

		val += i;
	}

	checksum = (checksum + val) & 0xFF;

	return val;
}

unsigned short CIntelHex::inhex4()
{
	unsigned short val = inhex2();
	val *= 256;
	val += inhex2();
	return val;
}

unsigned short CIntelHex::address() const
{
	return addr;
}

const unsigned char* CIntelHex::data() const
{
	return hex_data;
}

int CIntelHex::dataLength() const
{
	return nbytes;
}

