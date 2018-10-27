/*
Title:				PLANT.C

Function:			To stuff info into the object file

Author:				Bruce

Date:				4 Dec 90

Directory:

Calling sequence:	

Input parameters:

Output parameters:

Description:

Non-library routines used:

Possible error codes returned:

Revision history:

@*/

#pragma warning( disable : 4786)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "Plant.hpp"
#include "Segment.hpp"
#include "Expression.hpp"
#include "SymbolTable.hpp"
#include "Listing.hpp"
#include "Exception.hpp"
#include "Error.h"

#define SIMPLE(x) (x->type == t_number)

char Plant::xdigits[]={"0123456789ABCDEF"};

Plant::Plant()
{
	pass = 1;
	outfp = 0;

	// Initialise basic code and data segments
	currentSegment = new Segment("code");
	segments.addSegment(currentSegment);
	segments.addSegment(new Segment("data"));
}

Plant::~Plant()
{
	if(outfp)
	{
		fclose(outfp);
	}
}

/*************************************************************************/
/** STUFF_BYTE writes a byte to the object file in the form of a 2 	**/
/** digit hex number.													**/
/*************************************************************************/
void Plant::stuff_byte(int value)
{
	assert(this);
	if(pass==1) return;
	listing.add_byte(value);
	fputc(xdigits[(value&0xF0)>>4],outfp);
	fputc(xdigits[value&0x0F],outfp);
	return;
}

/*************************************************************************/
/** BUMP increments the appropriate (code or data ) counter by the 	**/
/** value passed to it 												**/ 
/*************************************************************************/
void Plant::bump(int nbytes)
{
	currentSegment->bump(nbytes);
	return;
}

/****************************************************************/
/** WRITE_BYTE													**/
/** writes a byte to the output file without incrementing		**/
/** the address counter										**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::write_byte(int value)
{
	if(pass==1) return;
	stuff_byte(value);
	return;
}

/*************************************************************************/
/** PLANT_BYTE writes a byte to the output file and bumps the pc		**/
/*************************************************************************/
void Plant::plant_byte(int value)
{
	bump(1);
	write_byte(value);
	return;
}


/****************************************************************/
/** WRITE_WORD													**/
/** writes a word to the output file (either ls or ms first)	**/
/** without incrementing the address counter.					**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::write_word(ADDRESS word)
{
	int i;
	ADDRESS mask;
	if(pass==1) return;

	if(littleEndian)
		{
		for(i=0;i<sizeof(ADDRESS);++i)
			{
			mask=0xFF<<(8*i);
			stuff_byte((int)((word&mask)>>(8*i)));
			}
		}
	else
		{
		for(i=0;i<sizeof(ADDRESS);++i)
			{
			mask=0xFF<<(8*(sizeof(ADDRESS)-1-i));
			stuff_byte((int) ((word&mask)>>(8*(sizeof(ADDRESS)-1-i))) );
			}
		}
	return;
}
/*************************************************************************/
/** PLANT_WORD writes a word to the output file. 						**/
/** If current->little_endian is true then the word is written ls byte	**/
/** first otherwise it is written ms byte first.						**/
/** The program counter is bumped by sizeof(ADDRESS)					**/
/*************************************************************************/
void Plant::plant_word(ADDRESS word)
{

	bump(sizeof(ADDRESS));
	if(pass==1) return;
	
	write_word(word);

	return;
}

/*************************************************************************/
/** PLANT_STRING plants a string in hex into the object file. The 	**/
/** trailing nul is not written. Used for planting data.		**/
/*************************************************************************/
void Plant::plant_string(const char *str)
{
	bump(strlen(str));
	if(pass==1) return;
	while(*str) stuff_byte(*str++);
	return;
}

/*************************************************************************/
/** PLANT_ASCII is used for planting ascii strings directly into the 	**/
/** object file. Used where a name or comment is needed.		**/
/*************************************************************************/
void Plant::plant_ascii(const char *str)
{
	if(pass==1) return;
	while(*str)
		{
		fputc(*str,outfp);
		++str;
		}
	return;
}

/*************************************************************************/
/** PLANT_TYPE plants a type specifier for an absolute address or 		**/
/** public entry.														**/
/*************************************************************************/
void Plant::plant_type(const Segment& seg)
{
	plant_ascii(".S");
	plant_ascii(seg.getName().c_str());
	fputc('\0',outfp);
	return;
}

/*************************************************************************/
/** PLANT_ADDR is used to plant an address that can be read by the 	**/
/** linker. Plant_addr ignores the little_endian flag			**/
/*************************************************************************/
void Plant::plant_addr(ADDRESS addr)
{
	int i;
	ADDRESS mask;

	if(pass==1) return;
	for(i=0;i<sizeof(ADDRESS);++i)
		{
		mask=0xFF<<(8*(sizeof(ADDRESS)-1-i));
		stuff_byte((int) ((addr&mask)>>(8*(sizeof(ADDRESS)-1-i))));
		}
	return;
}

/*************************************************************************/
/** PLANT_COMMENT plants a comment record in the object file		**/
/*************************************************************************/
void Plant::plant_comment(const char *string)
{
	plant_ascii("..");
	plant_ascii(string);
	fputc('\0',outfp);
	return;
}

/*************************************************************************/
/* PLANT_PUBLIC	 plants a public entry point specifier in the object**/
/** file. Note that this is only information for the linker and so 	**/
/** does not bump the address counter									**/
/*************************************************************************/
void Plant::plant_public(const Segment& seg, ADDRESS addr,const char *name)
{
	if(pass==1) return;
	plant_ascii(".P");
	plant_type(seg);
	plant_addr(addr);
	plant_ascii(name);
	fputc('\0',outfp);
	return;
}

/*************************************************************************/
/** PLANT_ORG plants an org record to tell the linker where the image	**/
/** should be located in store						**/
/*************************************************************************/
void Plant::plant_org(ADDRESS addr)
{
	if(pass==1) return;
	plant_ascii(".O");
	plant_addr(addr);
	return;
}



/*************************************************************************/
/** PLANT_RELOC plants a relocation marker to tell the linker that the	**/
/** object file is relocatable 						**/
/*************************************************************************/
void Plant::plant_reloc(void)
{
	segments.resetAddresses();

	if(pass==1) return;
	plant_ascii(".R");
	return;
}

/*************************************************************************/
/** PLANT_PHASE plants a phase record to tell the linker that the 		**/
/** address counter should be moved up to the next integral multiple	**/
/** of the phase argument												**/
/*************************************************************************/
void Plant::plant_phase(ADDRESS phase)
{
	if(pass==1) return;
	plant_ascii(".F");
	plant_addr(phase);
	return;
}

/*************************************************************************/
/** PLANT_ENDIAN plants a big-endian or little-endian record depending	**/
/** on current->little_endian. (Just for the record little-endian means	**/
/** that multi-byte values are stored least significant byte first 	**/
/*************************************************************************/
void Plant::plant_endian(void)
{
	if(pass==1) return;
	if(littleEndian)
		plant_ascii(".L");
	else
		plant_ascii(".B");
	return;
}


/****************************************************************/
/** PLANT_DEFS													**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_defs(int size)
{
	plant_ascii(".M");
	plant_addr(size);
	bump(size);
	return;
}


/****************************************************************/
/** PLANT_Expression_BYTE											**/
/** plants an expression as a byte. If it is a simple number 	**/
/** then the byte is duly planted, otherwise it is planted as a**/
/** full 8 bit expression.										**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_expr_byte(Expression *ex)
{
	assert(this);
	assert(ex);

	bump(1);
	ex->minimise();

	switch(ex->getType())
		{
		case Expression::t_number:
			write_byte(ex->getValue());
			break;
		default:
			plant_ascii(".E08[");
			plant_expr(ex);
			plant_ascii(" ]\n");
		}
	delete ex;
	return;
}

/****************************************************************/
/** PLANT_Expression_WORD											**/
/** plants an expression as a word. If the expression is a		**/
/** number it is planted as 2 bytes in the current endian 		**/
/** scheme, An address is planted as # followed by the offset	**/
/** (written ms-first). Anything more complicated is planted	**/
/** as a full 16 bit expression.								**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_expr_word(Expression *ex)
{
	bump(2);

	ex->minimise();

	if(pass == 2)	 /* only write object on second pass */
		{
		switch(ex->getType())
			{
			case Expression::t_number:
				write_word(ex->getValue());
				break;
			case Expression::t_addr:
				{
					Expression::AddressT addr;
					addr = ex->getAddress();
					fputc('#',outfp);
					plant_segment(addr.segment->getName().c_str());
					write_word(addr.addr);
					break;
				}
			default:
				plant_ascii(".E10[");	  /* hex 10 -> 16 bits */
				plant_expr(ex);
				plant_ascii(" ]\n");
				break;
			}
		}
	delete ex;
	return;
}

/****************************************************************/
/** PLANT_REL8													**/
/** stuffs an 8 bit relative jump into the object file			**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_rel8(Expression *ex)
{
	bump(1);
	ex->minimise();
	plant_ascii(".E08[");
	plant_expr(ex);
	plant_ascii(" $ -]");
	delete ex;
	return;
}

/****************************************************************/
/** PLANT_REL16												**/
/** plants an expression as relative to the program counter 	**/
/** by outputing a current pc ($) minus expr.					**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_rel16(Expression *ex)
{
	bump(2);
	ex->minimise();
	plant_ascii(".E10[");	  /* hex 10 -> 16 bits */
	plant_expr(ex);
	plant_ascii(" $ -]\n");
	delete ex;
	return;
}

/****************************************************************/
/** PLANT_ABS16												**/
/** should plant a word where a 16 bit absolute address is 	**/
/** expected. In actual fact it is no different from plant		**/
/** expr word as addresses are planted as addresses, numbers	**/
/** as numbers.												**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_abs16(Expression *ex)
{
	plant_expr_word(ex);
	return;
}

/****************************************************************
* Plant::plant_segment
* Plants a segment record that defines the following output as
* belonging to the named segment.
****************************************************************/
void Plant::plant_segment(const char *name)
{
	assert(this);
	assert(name);

	Segment* segment = segments.lookup(name);
	if(segment)
		setCurrentSegment(segment);
	else
		Exception::error(0); // TODO


	plant_ascii(".G");	// .S already used.
	plant_ascii(name);
	fputc('\0',outfp);
}

/****************************************************************
* 
****************************************************************/
Segment& Plant::getCurrentSegment() 
{
	assert(this);
	assert(currentSegment);
	return *currentSegment;
}

/****************************************************************
* 
****************************************************************/
void Plant::setCurrentSegment(Segment* segment) 
{
	assert(this);
	assert(segment);
	currentSegment = segment;
}

/****************************************************************
* 
****************************************************************/
void Plant::resetSegments()
{
	assert(this);
	segments.resetAddresses();
}

/****************************************************************
* 
****************************************************************/
//SegmentList& Plant::getSegments() 
//{
//	assert(this);
//	return segments;
//}

/****************************************************************
* 
****************************************************************/
//Listing& Plant::getListing() 
//{
//	assert(this);
//	return listing;
//}

/*********************************************************/
/** WRITE_PUBLIC writes all the public entry points	**/
/** to the object file.					**/
/*********************************************************/
void Plant::write_public(SymbolTable* symbols)
{
	assert(this);
	assert(symbols);

	if(!symbols->isEmpty())
	{
		plant_comment("\nPublic entry points:\n");

		for(SymbolTable::iterator iter = symbols->begin();
		iter != symbols->end();
		++iter) 
		{
			Symbol* sym = *iter;
			if(sym->getScope() == Symbol::_public)
				plant_public(sym->getSegment(), sym->getAddress(), sym->getName().c_str());
		}
	}
		
return;
}

void Plant::setPass(int pass)
{
	assert(this);
	this->pass = pass;
	listing.setPass(pass);
}

void Plant::openOutput(const char* path)
{
	assert(this);
	assert(path);

	if((outfp = fopen(path,"w"))==0)
		{
		Exception::error(DUFF_OP_FILE);
		}

}

void Plant::onNewLine(const char* line, int lineNo)
{
	assert(this);
	assert(line);

	listing.flush_line();
	listing.create_line(line, lineNo, *currentSegment);
}

void Plant::openListing(const char* path)
{
	assert(this);
	assert(path);

	listing.openOutput(path);
}

void Plant::listSymbolTable(const SymbolTable* symbols)
{
	assert(this);
	assert(symbols);
	listing.appendSymbolTable(symbols);
}
