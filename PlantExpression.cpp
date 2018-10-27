/*
File:				PLNTEXPR.C

Program/rev.:		ASSEM

Function:		  	To plant an expression

Author:	   		Bruce

Date:			   	10 July 92

Routines:

Input Parameters:

Output Parameters:

Error conditions:

Description:

Revision History:

*/

#pragma warning(disable : 4786)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "error.h"
#include "Plant.hpp"
#include "SymbolTable.hpp"
#include "Expression.hpp"

#define fatal(x) {printf("FATAL %s\n",x);}

/****************************************************************/
/** PLANT_ST_ENTRY												**/
/** plants a symbol table entry to the object file. Note that	**/
/** if an expression has been minimised, the equate will have	**/
/** been replaced by its value									**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_st_entry(const Symbol *st)
{
	assert(this);
	assert(st != NULL);

	switch (st->scope)
		{
		case Symbol::equate:
			plant_addr((int)st->getAddress());
			break;

		case Symbol::local:
		case Symbol::_private:
		case Symbol::_public:
			if(st->isRelocatable())
				{
				fputc('#',outfp);
				plant_type(st->getSegment());
				plant_addr((int)st->getAddress());
				}
			else
				plant_addr((int)st->getAddress());
			break;

		case Symbol::external:
			fprintf(outfp,"@%s ",st->getName().c_str());
			break;

		default:
			fatal("Illegal scope");
			break;
		}
	return;
}

/****************************************************************/
/** PLANT_OPERATOR												**/
/** plants a single operator to the output stream				**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_operator(Expression::e_operator op)
{
	char *str;

	switch (op)
		{
		case Expression::op_or:		str="|"; 	break;
		case Expression::op_xor:	str="^";	break;
		case Expression::op_and:	str="&";	break;
		case Expression::op_not:	str="~";	break;
		case Expression::op_eq:		str="=";	break;
		case Expression::op_ne:		str="<>";	break;
		case Expression::op_lt:		str="<";	break;
		case Expression::op_gt:		str=">";	break;
		case Expression::op_le:		str="<=";	break;
		case Expression::op_ge:		str=">=";	break;
		case Expression::op_plus:	str="+";	break;
		case Expression::op_minus:	str="-";	break;
		case Expression::op_mult:	str="*";	break;
		case Expression::op_div:	str="/";	break;
		case Expression::op_mod:	str="%";	break;
		case Expression::op_shr:	str=">>";	break;
		case Expression::op_shl:	str="<<";	break;
		case Expression::op_negate:	str="!";	break;
		case Expression::op_nop: 	str=""; 	break;
		default:
			fatal("Illegal operator");
			break;
		}
	fprintf(outfp,"%s ",str);
	return;
}

/****************************************************************/
/** PLANT_EXPR													**/
/** plants an expression in RPN format in the given size.		**/
/** Note that all number output by plant_expr are 				**/
/** ms	byte first- plant expr should only be used when its 	**/
/** output should be surrounded by expression delimiters [].	**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void Plant::plant_expr(Expression *ex)
{
	if(ex == 0) return;

	plant_expr(ex->getLeft());
	plant_expr(ex->getRight());

	switch(ex->getType())
		{
		case Expression::t_number:
			plant_addr(ex->getValue());
			assert(ex->getLeft() == 0);
			assert(ex->getRight() == 0);
			break;

		case Expression::t_addr:
			fputc('#',outfp);
			plant_type(*(ex->getAddress().segment));
			plant_addr(ex->getAddress().addr);
			assert(ex->getLeft() == 0);
			assert(ex->getRight() == 0);
			break;
		case Expression::t_symbol:
			plant_st_entry(ex->getSymbol());
			assert(ex->getLeft() == 0);
			assert(ex->getRight() == 0);
			break;
		case Expression::t_operator:
			plant_operator(ex->getOperator());
			break;
		default:
			fatal("Illegal expression type");
		}
	return;
}

