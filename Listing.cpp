/*
Title:			LISTING.C

Function:		To create assember listing files

Author:			Bruce

Date:			3 February 1991

Directory:

Calling sequence:	create_listing_line();
			add_listing_byte(int byte);
			flush_listing_line();
			
Input parameters:

Output parameters:

Description:
			format of listing line is :
			lll,T:xxxx xxyyzzppqq source line.........
			where lll is the line number
			T is C or D for code or data
			xxxx is the current code or data position
			xxyyzzppqq are the possible output

Non-library routines used:

Possible error codes returned:

Revision history:

@*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include "Listing.hpp"
#include "Segment.hpp"
#include "SymbolTable.hpp"
#include "Exception.hpp"
#include "Error.h"

#define MAXOBJ 5

#define TYPE 5
#define COUNT 7
#define OBJECT 14
#define SOURCE 25

#define TABSIZE 4

Listing::Listing()
{
	pass = 1;
	objcount = 0;
	enabled = true;
	listfp = 0;
	list_buffer[0]='\0';
}

Listing::~Listing()
{
	if(listfp)
	{
		fclose(listfp);
	}
}


/*****************************************************************/
/** CREATE_LISTING_LINE sets up a listing line with most of the **/
/** information in it. The assember output is shoved in the 	**/
/** buffer by add_listing_byte();				**/
/*****************************************************************/
#define TABSIZE 4
void Listing::create_line(const char* source, int line, const Segment& seg)
{
	if(pass==1) return;
	if(!enabled) return;

	memset(list_buffer,' ',LINELEN);
	list_buffer[LINELEN]='\0';
	objcount=0;
	int preamble = sprintf(list_buffer,"%3.3d,%4s:%4.4x            ",line,
					seg.getName().c_str(),
					seg.getAddress());

	int count=0;
	char* there = list_buffer+preamble;
	for(const char* here = source; *here && (count <40) ; ++here)
		{
		if(*here == '\t')
			{
			int newcount=((count+TABSIZE)/TABSIZE)*TABSIZE;
			while(count != newcount)
				{
				*there++ = ' ';
				++count;
				}
			}
		else
			{
			*there++ =*here;
			++count;
			}
		}
	*there= '\0';
	return;
}

/*****************************************************************/
/** FLUSH_LISTING_LINE writes the listing buffer to the listing	**/
/** file.							**/
/*****************************************************************/
void Listing::flush_line()
{
	if(pass==1) return;
	if(!enabled) return;
	if(*list_buffer == 0) return;
	fputs(list_buffer,listfp);
	fputc('\n',listfp);
	return;
}

/*****************************************************************/
/** ADD_LISTING_BYTE stuffs an output byte into the listing	**/
/** buffer. If there is no room for the output byte then the	**/
/** buffer is flushed to the listing file and a continuation	**/
/** line is created. 						**/
/*****************************************************************/
void Listing::add_byte(int byte)
{
	char vbuff[3];
	if((pass==1)||(!enabled)) return;
	byte&=0xFF;
	sprintf(vbuff,"%2.2x",byte);
	if(objcount==MAXOBJ)
		{
		flush_line();
		memset(list_buffer,' ',LINELEN);
		list_buffer[LINELEN]='\0';
		*list_buffer='+';
		objcount=0;
		}
	*(list_buffer+OBJECT+2*objcount)=vbuff[0];
	*(list_buffer+OBJECT+2*objcount+1)=vbuff[1];
	++objcount;
	return;
}

void Listing::appendSymbolTable(const SymbolTable* symbols)
{
	assert(this);
	assert(symbols);

	symbols->dump_sym_tab(listfp);
}

void Listing::setPass(int pass)
{
	assert(this);
	this->pass = pass;
}

void Listing::openOutput(const char* path)
{
	assert(this);
	assert(path);

	if(listfp != 0)
	{
		fclose(listfp);
	}

	if((listfp=fopen(path,"w"))==NULL)
		{
		Exception::error(DUFF_OP_FILE);
		}
	enabled = (listfp != 0);
}

