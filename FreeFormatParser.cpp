// FreeFormatParser.cpp: implementation of the FreeFormatParser class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )
#include <assert.h>
#include <string.h>

#include "FreeFormatParser.hpp"
#include "InputLine.hpp"
#include "Plant.hpp"
#include "Exception.hpp"
#include "error.h"

char FreeFormatParser::ffmtstart[]={"0123456789#'+("};

#define ffmt(c) ((c!='\0')&&(strchr(ffmtstart,c)!=NULL))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FreeFormatParser::FreeFormatParser(InputLine* input, Plant* _plant)
: line(input)
, plant(_plant)
{
	assert(input);
	assert(_plant);
}

FreeFormatParser::~FreeFormatParser()
{

}



void FreeFormatParser::do_free_fmt_bit(int pass)
{
	assert(this);
	switch(line->current())
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			plant->plant_expr_byte(line->get_expression());
			break;
		
		case '#':
			line->skip();		/* skip # */
			plant->plant_expr_word(line->get_expression());
			break;

		case '\'':
			{
				char* string = line->get_string();
				plant->plant_string(string);
				delete [] string;
				break;
			}

		case '+':
			line->skip();
			plant->plant_expr_byte(line->get_expression());
			break;

		case '(':
			line->skip();		/* skip leading ( */
			plant->plant_expr_byte(line->get_expression());
			if(line->current() != ')' )
			{
				if (pass==1)
					Exception::error(PARENTHESES, line);
			}
			else
			{
				line->skip();	/* skip trailing ) */
			}
			break;
		default:
			plant->plant_expr_byte(line->get_expression());
			break;
		}
	return;
}


bool FreeFormatParser::parse(int pass)
{
	assert(this);
	line->skip_space();
	if(!ffmt(line->current())) 
		return false;
	
	bool stop = false;
	do
		{
		line->skip_space();
		do_free_fmt_bit(pass);
		line->skip_space();
		if(line->current() == ',')
			{
			line->skip();		/*skip the comma*/
			stop = false;
			}
		else
			stop = true;
		}
	while(!stop);
	return true;
}
