/*
Title:					InputLine.cpp
						(was GET.C)

Function:				to get various bits and bobs from the input
						file

Author:				Bruce

Date:					5 December 90

Directory:	

Calling sequence:		name=get_name();		
						value=get_expression();
						name=extern_name();
Input parameters:

Output parameters:

Description:

Non-library routines used:

Possible error codes returned:

Revision history:

@*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


#include "InputLine.hpp"
#include "error.h"
#include "Segment.hpp"
#include "Expression.hpp"
#include "SymbolTable.hpp"
#include "ExpressionEvaluator.hpp"
#include "Exception.hpp"




char InputLine::duff_fcs[] = "~[]<|>+=;,/"; /* chars not allowed in a fn. */
char InputLine::numsyms[] = "0123456789abcdefABCDEFHhOoQq.";


InputLine::InputLine(const char* path) 
: source_path(path)
{
	infp = 0;			/* input file pointer */
	start_of_line = 0L;	/* pointer into file*/
	line_no = 0;;		/* in current file */
	eof_flag = false;

	bstart = 0;
	buff = 0;

	radix = 10;

	pass = 1;
	currentSegment = 0;

	infp = fopen(path,"r");
	eof_flag = (infp == 0);

}

InputLine::~InputLine() {
}


/*
* Sets the current pass.  Needed for error reporting as some errors
* only make sense on the first or second passes.
*/
void InputLine::setPass(int pass) {
	assert(this);
	this->pass = pass;
}

/*
* Sets the current memory segment.  Needed to allow the $ operator to work
*/
void InputLine::setSegment(Segment * seg){
	assert(this);
	assert(seg);
	currentSegment = seg;
}

/*
* sets the current symbol table.  Used for resolution of symbols.
*/
void InputLine::setSymbolTable(SymbolTable* table) {
	assert(this);
	assert(table);
	symbolTable = table;
}

/*************************************************************************/
/** IS_SYM returns true if the current line starts with the given		**/
/** symbol otherwise false. If the symbol matches it is skipped in the	**/
/** input buffer. Note that to cope with discriminating between 		**/
/** say, LD A,I and LD A,INTVECT it is not enough just to look for a 	**/
/** straight match. Instead, the symbol string is defined as consisting**/
/** of punctuation characters and tokens. Tokens are defined as a 		**/
/** continuous run of alphanumerics or full stops. Punctuation is 		**/
/** matched on a single character basis, tokens however must match		**/
/** over their entire lengths. Note that spaces between punctuation	**/
/** characters and tokens in the input buffer are ignored.				**/
/*************************************************************************/
bool InputLine::is_sym(const char *sym)
{
	
	size_t stlen,itlen;	/* symbol token length and input token length */

	skip_space();
	
	char* here=lsptr;
	while(*sym)
		{
		while(isspace(*here) && (*here != '\0'))
			++here;

		if (istok(*sym)) 					/* if we are looking for a token */
			{
			char* tokend=here;  			/* find end of token in input buff */
			while(istok(*tokend))
				++tokend;
			itlen=tokend-here;				/* get length of input token */

			const char* stend=sym;			/* find end of token in symbol */
			while(istok(*stend))
				++stend;
			stlen=stend-sym;				/* and therefore length of symbol tok */

			if (itlen != stlen)				/* if lengths don't match... */
				return(false);				/*  stop here */

			while(here != tokend)			/* check token in sym matches */
				{							/* complete token in input buff */
 				if(toupper(*here)==toupper(*sym))
					++here,++sym;
				else
					return(false);
				}
			}
		else								/* is is not token character */
			{
			while(isspace(*here) && (*here != '\0'))
				++here;
			if (*here == *sym)
				++here,++sym;
			else
				return(false);
			}
		}
		lsptr=here;
	return(true);
}

/*************************************************************************/
/**** BLANK_LINE  returns true if the input buffer is empty or only 	**/
/** contains spaces 													**/
/*************************************************************************/
bool InputLine::blank_line()
{
	char* here=lsptr;	/* current start of line */
	while(*here)
		{
		if (!isspace(*here)) return(false);
		++here;
		}
	return(true);
}

/*************************************************************************/
/***** STRIP_COMMENTS strips any comment from the end of line line 	**/
/***** ; starts a comment, \ or ^ escapes any character following and	**/
/***** ' ' quotes text 							**/
/*************************************************************************/

void InputLine::strip_comments(char cchar)
{
	char* here=lsptr;		/* current start of line */

	enum st state=initial;
	while(state!=finish)
		{
		switch(state)
			{
			case initial:
				if((*here==cchar)||(*here=='\0'))
					{
					*here='\0';		/* cut off comment */
					state=finish;
					break;
					}
				if(*here=='\'')
					{
					state = instring;
					break;
					}
				break;
			case instring:
				if(*here=='\'')
					{
					state = initial;
					break;
					}
				if((*here=='\\')||(*here=='^'))
					{
					state = escape;
					break;
					}
				if(*here=='\0')
					{
					if(pass==1) Exception::error(UNMATCHED_QUOTE, this);
					state=finish;
					break;
					}
				break;
			case escape:
				if(*here=='\0')
					{
					if(pass==1) Exception::error(UNMATCHED_QUOTE, this);
					state=finish;
					break;
					}
				state = instring;	
				break;		/* do nothing with escaped char */
			default:
				Exception::fatal("Illegal state in InputLine fsm");
				break;
			} /*endswitch*/
		++here;				/* next character in buffer */
		} /*endwhile */
	return;	
}

/*************************************************************************/
/****** STRIP_LEADING strips leading characters from the input buffer	**/
/*************************************************************************/

void InputLine::strip_leading(char ch)
{
	while(*lsptr)
		{
		if(*lsptr == ch) 
			++lsptr;
		else
			break;
		}
	return;
}

/*************************************************************************/
/****** SKIP_SPACE strips leading spaces,tabs or what have you *****/
/*************************************************************************/

void InputLine::skip_space()
{
	while (*lsptr)
		if(isspace(*lsptr))
			++lsptr;
		else
			break;
	return;
}

/*************************************************************************/
/** CHECK_LINE checks the input buffer and if it is empty reads in 	**/
/** input lines until it is not empty. Note that check_line is only	**/
/** intended for use by the processor description due to the comment	**/
/** character used.													**/
/*************************************************************************/
void InputLine::check_line()
{
	while(blank_line())
		{
		read_line();
		strip_comments('/');
		}
	return;
}


/********************************************************************/
/** GET_NAME returns a valid symbol table name in new-ed memory ***/
/********************************************************************/

char* InputLine::get_name()
{
	skip_space();
	char* here = lsptr;
	int len=0;
	if((*here=='.')||isalpha(*here))
		{
		++here;
		++len;
		}
	else
		{
		return(NULL);
		}
	while((*here=='.')||isalnum(*here))
		{
		++here;
		++len;
		}

	char* name = new char[len+1];
	strncpy(name,lsptr,len);
	name[len]='\0';
	lsptr=here;
	return name;
}

/***************************************************************************/
/** GET_STRING reads in a string delimited by single quotes and returns it */
/** in a piece of new-ed memory without the quotes			  **/
/***************************************************************************/
char* InputLine::get_string()
{
	skip_space();
	char* here = lsptr;
	if(*here != '\'')
		{
		if (pass==1) Exception::error(INVALID_STRING, this);
		return(0);
		}
	++here;				/* skip opening quote */
	char* start=here;	/* point at first char in string */

	/* how many characters in the string ? */

	int len=0;
	enum st state=instring;
	while(state!=finish)
		{
		switch(state)
			{
			case instring:
				if(*here=='\'') { state=finish; break; }
				if(*here=='\\') { state=escape; break;}
				if(*here=='^')	{ state=caret; break;}
				if(*here=='\0')
					{
					if(pass==1) Exception::error(UNMATCHED_QUOTE, this);
					state=finish;
					break;
					}
				break;
			case escape:
				if(*here=='\0')
					{
					if(pass==1) Exception::error(UNMATCHED_QUOTE, this);
					state=finish;
					break;
					}
				state=instring;
				break;		
			case caret:
				if(*here=='\0')
					{
					if(pass==1) Exception::error(UNMATCHED_QUOTE, this);
					state=finish;
					break;
					}
				state=instring;
				break;		
			default:
				Exception::fatal("Illegal state in InputLine fsm");
				break;
			} /*endswitch*/
		++here;				/* next character in buffer */
		++len;
		} /*endwhile */


	/* assert- here points just past trailing ' of a valid string
	 (or trailing \0 if the string was not terminated).
	  len has the number of characters in the string+1 */

	lsptr=here;	/* line now starts after trailing quote */

	char* string = new char[len];	/* get (at least) enough storage */

	here=start;		/* copy from input buffer */
	char* there=string;		/* to destination string */

	state=instring;

	while(state!=finish)
		{
		switch(state)
			{
			case instring:
				if(*here=='\'') { *there='\0'; state=finish; break; }
				if(*here=='\\') { state=escape; break;}
				if(*here=='^')	{ state=caret; break;}
				if(*here=='\0') { state=finish;	break;}
				*there++=*here;
				break;
			case escape:
				if(*here=='\0') { *there='\0'; state=finish; break; }
				*there++ = *here;
				state=instring;	
				break;		
			case caret:
				if(*here=='\0')	{ *there='\0'; state=finish; break; }
				*there++ = (char) (128|(*here));
				state=instring;
				break;
			default:
				Exception::fatal("Illegal state in InputLine fsm");
				break;
			} /*endswitch*/
		++here;				/* next character in input buffer */
		} /*endwhile */

	return(string);
}


/**************************************************************************/
/** GET_FILENAME reads in a filename and returns it as a new-ed bit of **/
/** memory. Note that there is almost no checking for a valid filename   **/
/** no great catastrophy- the program won't be able to open it if the    **/
/** filename is completely weird					 **/
/**************************************************************************/
char* InputLine::get_filename()
{
	skip_space();
	
	char* start = lsptr;
	char* here = lsptr;

	if(!isfilechar(*here))
		{
		Exception::error(INVALID_FILENAME, this);
		return(0);
		}
	int len=0;
	while(isfilechar(*here))
		{
		++here;
		++len;
		}
	char* fn = new char[len+1];
	strncpy(fn,start,len);
	fn[len]='\0';
	return(fn);
}

/*************************************************************************/
/** GET_INT reads an integer in. First get_int tries to read in an	**/
/** integer in the current base. If that fails then get_int looks at 	**/
/** last character to see if it changes the base. If there is a base	**/
/** specifier then get_int tries to parse the rest of the int in the	**/
/** new base.								**/
/*************************************************************************/
int InputLine::get_int()
{
	skip_space();
	
	char* here = lsptr;
	char* start = lsptr;

	while(isnumsym(*here)) ++here;
	char* finish=here;				/* one past last number character */

	bool invalid=false;
	int accum=0;

	for(here=start;here!=finish;++here)
		{
		int c=xlat(*here);
		if(isxdigit(*here) && (c < this->radix)) /* ok in current base ?*/
			{
			accum *= this->radix;
			accum+=c;
			}
		else
			{
			invalid=true;
			break;
			}
		}

	if (!invalid) 
		{
		lsptr=finish;
		return(accum);
		}
	
	/* OK so number not valid in current base- look to last char to see if it
	has a base suffix */

	int radix = this->radix;

	switch (toupper(*(finish-1)))
		{
		case 'D':
		case '.': radix=10; break;
		case 'O':
		case 'Q': radix=8; break;
		case 'B': radix=2; break;
		case 'H': radix=16; break;
		default:
			if(pass==1) Exception::error(INVALID_NUMBER, this);
			return(0);
		}
	
	/* now try again with the new base */
	invalid=false;
	accum=0;

	for(here=start;here!=(finish-1);++here)
		{
		int c=xlat(*here);
		if(isxdigit(*here)&&(c<radix))
			{
			accum *= radix;
			accum += c;
			}
		else
			{
			invalid=true;
			break;
			}
		}
	
	if(invalid)
		{
		if(pass==1) Exception::error(INVALID_NUMBER, this);
		return(0);
		}
	lsptr=finish;
	return(accum);
}

/*************************************************************************/
/** GET_NUMBER is used to get an integer which can be expressed as a 	**/
/** normal number (i.e. set of digits), the program counter ($), a 	**/
/** symbolic name or a string (in which case the ascii value of the	**/
/** first character is returned					 					**/
/*************************************************************************/
Expression* InputLine::get_number()
{
	char *name;
	char *string;
	int value;

	skip_space();
	char* here = lsptr;

	if(*here == '$')		/* use current program counter */
		{
		lsptr=here+1;	/* skip the $ */
		assert(currentSegment);
		return Expression::addrto_expr(currentSegment);
		}

	if(isdigit(*here))	/* a real, honest to god integer ? */
		{
		return(Expression::ito_expr(get_int()));
		}

	if (*here=='\'')	/* a character constant ? */
		{
		string=get_string();	/* suck in a complete string */
		value=*string;		/* but only use the first character */
		delete [] string;
		return Expression::ito_expr(value);
		}
	
	/* none of the above so it must be a symbol table entry */
	name=get_name();
	if(name==0) 
		{
		if (pass==1) Exception::error(FUNNY_NUMBER, this);
		return(0);
		}
	
	assert(symbolTable);
	Symbol* symbol = symbolTable->find(name);
	if(symbol != 0)
		{
		if(symbol->isExternal()) 
			{
			log_extern_name(symbol->name);
			}
		delete [] name;
		return(Expression::symto_expr(symbol));
		}

	/* ok so name is not in either symbol table at the moment. */
	if (pass==2) Exception::error(UNRESOLVED_SYMBOL, this);

	delete [] name;
	return(0);					/* dummy as not known */
}

/****************************************************************************/
/* GET_EXPRESSION */
/*****************************************************************************/
Expression* InputLine::get_expression()
{
	ExpressionEvaluator eval(this, pass);
	return eval.result();
}

/*************************************************************************/
/** GET_KEYWORD looks for a keyword at the start of the input buffer. 	**/
/** if one is found then its code is returned otherwise -1 is returned 	**/
/*************************************************************************/
int InputLine::get_keyword(KEYMATCH keys[])
{
int i;
for(i=0;keys[i].name!=NULL;++i)
	{
	if(is_sym(keys[i].name))
		/* found a keyword */
		{
		return(keys[i].key);
		}
	}
return(-1);
}


/*****************************************************************/
/** SKIP_LABEL looks for a valid label at the start of the 	**/
/** input buffer. If one is found it is skipped and its start	**/
/** position (as a ptr) is returned.				**/
/*****************************************************************/
char* InputLine::skipLabel()
{

	/** look for a valid label at the start of the line */
	char* here = lsptr;		/* current start of input line */
	if((*here=='.')||isalpha(*here)) /* valid start ? */
		++here;			/* yes so skip it */
	else
		return 0;		/* no so give up now */

	while ((*here=='.')||isalnum(*here)) ++here;	/* skip over remainder */
	
	if(*here != ':') return 0;	/* valid label must have trailing : */

	/** here now points to the trailing : of a valid label */
	*here++='\0';					/* make label into a string */
	char* label = lsptr;	    	/* note where it starts */
	lsptr=here;						/* start i/p buffer after the label */

	/* label now points to a null terminated label in the buffer and current->lsptr
	points to the new start of the input buffer after the label. */

	return label;
}

/*************************************************************************/
/** SAVE_BUFFER copies the input buffer into new-ed memory and sets	**/
/** the line pointer so that all the input comes from the copy		**/
/*************************************************************************/

void InputLine::save_buffer()
{
	assert(bstart==NULL);			/* no multiple save !*/
	bstart=lsptr;
	buff = new char[strlen(lsptr)+1];	/* mem to hold current buffer */
	strcpy(buff,lsptr);				/* take a copy */
	lsptr=buff;						/* and start reading from the copy */
	return;
}

/*************************************************************************/
/** FREE_BUFFER frees up the input buffer copy and restores the old one	**/
/*************************************************************************/
void InputLine::free_buffer()
{
	assert(bstart!=NULL);
	lsptr=bstart;			/* point to normal buffer */
	bstart=0;
	delete [] buff;
	return;
}


/****************************************************************
* READ_LINE													
* Parameters:												
* Returns: true if not end of file. 
* so we can write:
* while(line->read_line())
*	process_input();
****************************************************************/
bool InputLine::read_line()
{
	char *buffptr;
	int c;

	if(eof_flag) 
		return false;

	++line_no;
	start_of_line=ftell(infp);
	lsptr=linebuff;
	buffptr=linebuff;

	while(true)
		{
		c=fgetc(infp);
		if (c==EOF)
			{
			eof_flag=true;
			break;
			}
		if(c=='\n') break;
		if(buffptr==linebuff+LINEBUFFSIZE-1)
			{
			Exception::error(LINE_TOO_LONG, this);
			/* bin remaining line */
			while((c=fgetc(infp))!=EOF)
				if(c=='\n') break;
			break;
			}
		*buffptr++= (char)c;
		}
	*buffptr='\0';

	fireOnNewLine();

	return !eof_flag;
}

/****************************************************************/
/** RESET_INPUT												**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void InputLine::reset_input()
{
fseek(infp,0L,SEEK_SET);
line_no=0;
eof_flag=false;
}

/****************************************************************
* InputLine::fireOnNewLine
* Fires the onNewLine event of any registered listeners.
****************************************************************/
void InputLine::fireOnNewLine()
{
	assert(this);
	for(ListenersT::iterator iter = listeners.begin();
	iter != listeners.end();
	++iter)
	{
		InputLineListener* listener = *iter;
		listener->onNewLine(linebuff, line_no);
	}
}

/****************************************************************
* InputLine::registerListener
****************************************************************/
void InputLine::registerListener(InputLineListener* listener)
{
	assert(this);
	assert(listener);
	listeners.push_back(listener);
}

/****************************************************************
* InputLine::removeListener
****************************************************************/
void InputLine::removeListener(InputLineListener* listener)
{
	assert(this);
	assert(listener);
	listeners.remove(listener);
}
