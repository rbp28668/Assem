/*
Title:				EXPR.C

Function:			To parse arithmetic and logical expressions

Author:			Bruce

Date:				20 December 90

Directory:

Calling sequence:	init_expression();
					expression();
					value=expression_result();

Input parameters:	none

Output parameters:	expression_result returns an integer

Description:		This is a recursive descent parser that implements
					the operator precedence described below. Note that in
					this implementation no attempt has been made to 
					eliminate tail recursion.
			
					OP		Description			Priority
					+		unary plus			7
					-		unary minus			7
					NOT		unary not			7
					*		multiplication		6
					/		division			6
					SHR,>>	shift right			6
					SHL,<<	shift left			6
					+		addition			5
					-		subtraction			5
					EQ,=	equality			4
					NE,<>	inequality			4
					GT,>	greater than		4
					LT,<	less than			4
					GE,>=	greater or equal	4
					LE,<=	less or equal		4
					AND		logical and			2
					OR		logical or			1
					XOR		logical exclusive or1
			
Non-library routines used:

Possible error codes returned:

Revision history:
Unary not moved from priority 3 up to priority 7

@*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "expression.hpp"
#include "SymbolTable.hpp"
#include "Segment.hpp"

//#include "assem.h"
//#include "extern.h"
#include "error.h"


/** PRODUCE takes a string, the recursion function name and the operator **/
/** name and expands the macro to check for the string and call the other **/
/** two bits and return if the string matched **/
#define produce(s,f,a) if(is_sym(s)){f(); a; return;}

#ifdef TRUE
#undef TRUE
#undef FALSE
#endif

#define TRUE ~0
#define FALSE 0

/** OPERATOR MACROS **/

#define NORMAL	1


#if NORMAL
#define OR 	push(binary_op(pop(),pop(),op_or))
#define XOR	push(binary_op(pop(),pop(),op_xor))
#define AND	push(binary_op(pop(),pop(),op_and))
#define NOT	push(unary_op(pop(),op_not))
#define EQ		push(binary_op(pop(),pop(),op_eq))
#define NE		push(binary_op(pop(),pop(),op_ne))
#define LT		push(binary_op(pop(),pop(),op_lt))
#define GT		push(binary_op(pop(),pop(),op_gt))
#define LE		push(binary_op(pop(),pop(),op_le))
#define GE		push(binary_op(pop(),pop(),op_ge))
#define PLUS	push(binary_op(pop(),pop(),op_plus))
#define MINUS	push(binary_op(pop(),pop(),op_minus))
#define MULT	push(binary_op(pop(),pop(),op_mult))
#define DIV	push(binary_op(pop(),pop(),op_div))
#define MOD	push(binary_op(pop(),pop(),op_mod))
#define SHR	push(binary_op(pop(),pop(),op_shr))
#define SHL	push(binary_op(pop(),pop(),op_shl))
#define NEGATE	push(unary_op(pop(),op_negate))
#define NO_OP

#else

#define OR 	printf("OR ")
#define XOR	printf("XOR ")
#define AND	printf("AND ")
#define NOT	printf("NOT ")
#define EQ		printf("EQU ")
#define NE		printf("NE ")
#define LT		printf("LT ")
#define GT		printf("GT ")
#define LE		printf("LE ")
#define GE		printf("GE ")
#define PLUS	printf("+ ")
#define MINUS	printf("- ")
#define MULT	printf("* ")
#define DIV	printf("/ ")
#define MOD	printf("MOD ")
#define SHR	printf(">> ")
#define SHL	printf("<< ")
#define NEGATE	printf("NEGATE ")
#define NO_OP

#endif

// Macros for minimising an expression.
#define COMBINE(x,op) (x->data.value = x->left->data.value op x->right->data.value)
#define RELATIONAL(x,op) (x->data.value = (x->left->data.value op x->right->data.value)?-1:0)

#define fatal(s) { printf(s); }

Expression::Expression() {

}

Expression::~Expression() {
	delete left;
	delete right;
}

/****************************************************************/
/** UNARY_OP													**/
/** takes a unary operator and its operand and combines them	**/
/** into a compound expression									**/
/** Parameters:													**/
/** Returns: 													**/
/****************************************************************/
Expression::Expression(Expression *l, enum e_operator op)
{

	this->type = t_operator;
	this->data.op = op;
	this->left = l;
	this->right = NULL;
}

/****************************************************************/
/** BINARY_OP													**/
/** takes 2 operands and a binary operator and makes a compound**/
/** expression from them										**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression::Expression(Expression *l, Expression *r, enum e_operator op)
{
	this->type = t_operator;
	this->data.op = op;
	this->left = l;
	this->right = r;
}


/**====================================================================**/
/** CONVERSION ROUTINES - Convert numbers, addresses and symbols to	**/
/** expressions.														**/
/**====================================================================**/

/****************************************************************/
/** ITO_EXPR													**/
/** Converts an integer to an expression						**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* Expression::ito_expr(int val)
{
	Expression* expr = new Expression();
	expr->type = t_number;
	expr->data.value = val;
	expr->left=NULL;
	expr->right=NULL;

	return expr;

}

/****************************************************************/
/** ADDRTO_EXPR												**/
/** Converts an address to an expression						**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* Expression::addrto_expr(Segment* segment)
{
	assert(segment);

	Expression* expr=new Expression();
	
	expr->type = t_addr;
	
	expr->data.address.segment = segment;
	expr->data.address.addr = segment->getAddress();
	expr->left=0;
	expr->right=0;

	return expr;

}

/****************************************************************/
/** SYMTO_EXPR													**/
/** Converts a symbol table entry to an expression				**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* Expression::symto_expr(Symbol *sym)
{
	Expression* expr = new Expression();
	expr->type = t_symbol;
	expr->data.symbol = sym;
	expr->left=0;
	expr->right=0;

	return expr;

}

/****************************************************************/
/** RESOLVE_SYMBOLS											**/
/** takes an expression and, as far as possible, replaces any	**/
/** symbol table references with addresses or numbers			**/
/** Parameters:												**/
/** 	EXPR *ex is the expression to be resolved.				**/
/** Returns: 													**/
/** 	nothing ( the expression is altered in place)			**/
/****************************************************************/
void Expression::resolve_symbols()
{
	if(left != 0)
		left->resolve_symbols();
	
	if(right != 0)
		right->resolve_symbols();

	if(type == t_symbol)
		{
		Symbol* sym = data.symbol;
		switch(sym->scope)
			{
			case Symbol::equate:
				if(! sym->resolved) break;
				data.value = (int) sym->address;
				type = t_number;
				break;
			case Symbol::local:
			case Symbol::_private:
			case Symbol::_public:
				if(! sym->isResolved()) break;

				type = t_addr;
				data.address.segment = &(sym->getSegment()); 
				data.address.addr = sym->getAddress();
				
				break;
			case Symbol::external:
				break;
			default:
				fatal("Illegal symbol scope");
			}
		}
	return;
}

/****************************************************************/
/** COMBINE_NUMBERS											**/
/** combines binary operators iff both operands are numbers 	**/
/** and combines unary operators iff its operand is a number.	**/
/** Parameters:												**/
/** 	EXPR *ex is the root of an expression. The root node	**/
/** 	must be an operator.									**/
/** Returns: 													**/
/** 	true if the root operator could be combined with its	**/
/** roots, false otherwise.									**/
/****************************************************************/
bool Expression::combine_numbers()
{
	bool done=false;

	assert(type == t_operator);
	if(left != 0 && right != 0)	/* is a binary op */
		{
		if(left->type==t_number &&			/* both operands are numbers*/
			right->type == t_number)
			{
			switch(data.op)
				{
				case op_or:  	COMBINE(this,|);		break; 	
				case op_xor:  	COMBINE(this,^);		break;	
				case op_and:  	COMBINE(this,&);		break;	
				case op_not:  	fatal("Unary op");		break;	
				case op_eq:  	RELATIONAL(this,==);	break;	
				case op_ne:  	RELATIONAL(this,!=);	break;	
				case op_lt:  	RELATIONAL(this,<);		break;	
				case op_gt:  	RELATIONAL(this,>);		break;	
				case op_le:  	RELATIONAL(this,<=);	break;	
				case op_ge:  	RELATIONAL(this,>=);	break;	
				case op_plus:  	COMBINE(this,+);		break;	
				case op_minus: 	COMBINE(this,-);		break;	
				case op_mult:  	COMBINE(this,*);		break;	
				case op_div:  	COMBINE(this,/);		break;	
				case op_mod:  	COMBINE(this,%);		break;	
				case op_shr:  	COMBINE(this,>>);		break;	
				case op_shl:  	COMBINE(this,<<);		break;	
				case op_negate:	fatal("Unary op");	break;
				case op_nop:  	break;	
				default:
					fatal("Illegal operator");
				}
			type = t_number;
			delete left;
			delete right;
			left = 0;
			right = 0;
			done=true;
			}
		}
	else 		/* is an unary operator */
		{
		if(left->type == t_number)
			{
			switch(data.op)
				{
				case op_not:
					data.value = ~left->data.value;
					break;
				case op_negate:
					data.value = 0-left->data.value;
					break;
				}
			type = t_number;
			delete left;
			left = 0;
			done=true;
			}
		}
	return(done);
}

/****************************************************************/
/** COMBINE_ADDRESES											**/
/** takes an expression with an operator at the root and tries	**/
/** to combine the operator with its operands iff both operands**/
/** are addresses. Note that only operations which involve 	**/
/** subtraction are allowed as the load_address's inherent		**/
/** in an address will cancel									**/
/** Parameters:												**/
/** 	EXPR *ex	is a pointer to an expression with an 		**/
/** 	operator as its root node.								**/
/** Returns: 													**/
/** 	true if the root operator could be combined with its	**/
/**	roots, false otherwise.									**/
/****************************************************************/
bool Expression::combine_addresses()
{
	bool done = false;
	assert(type  == t_operator);
	if(left != 0 && right != 0)	/* is a binary operator */
		{
		// Only valid if both sides are addresses and the addresses
		// are in the same segment.
		if((left->type == t_addr && right->type == t_addr) && 
			(left->data.address.segment == right->data.address.segment))
			{
			switch(data.op)
				{
				case op_minus: 	COMBINE(this,-);		break;	
				case op_eq:  	RELATIONAL(this,==);	break;	
				case op_ne:  	RELATIONAL(this,!=);	break;	
				case op_lt:  	RELATIONAL(this,<);		break;	
				case op_gt:  	RELATIONAL(this,>);		break;	
				case op_le:  	RELATIONAL(this,<=);	break;	
				case op_ge:  	RELATIONAL(this,>=);	break;	
				}
			type = t_number;
			delete left;
			delete right;
			left = 0;
			right = 0;
			done=true;
			} 					/* endif both leaves are addresses */
		} 					/* endif binary operator */
	return(done);
}
/****************************************************************/
/** MINIMISE_EXPRESSION										**/
/** is a recursive routine that tries to minimise an expression**/
/** as far as it can. If the root type is an operator then it	**/
/** tries to minimise both its branches. If both its branches	**/
/** are then numbers (or in some cases addresses) it can then	**/
/** combine them with its operator.							**/
/** 															**/
/** Parameters:												**/
/** 	EXPR *ex is the expression to be minimised.				**/
/** 															**/
/** Returns: 													**/
/** 	nothing, the expression is minimises in place.			**/
/****************************************************************/
void Expression::minimise()
{
	if(this == 0)
		return;

	left->minimise();
	right->minimise();

	if(type == t_operator)
		{
		if(!combine_numbers())
			combine_addresses();
		}
	return;
}


/****************************************************************/
/** RESOLVE_EXPR												**/
/** tries to resolve an expression to an integer.				**/
/** Parameters:												**/
/** Returns: 													**/
/** true if the expression can be resolved, false if it can not**/
/****************************************************************/
bool Expression::resolve_expr(int& val)
{
	assert(this);

	minimise();

	if(type == t_number)	/* expr has been resolved to a number */
		{
		val = data.value;
		return true;
		}

	val = -1;
	return false;
}


/****************************************************************/
/** clone (was dup_expr)    									**/
/** makes a duplicate copy of an expression.					**/
/** Parameters:												**/
/** 	EXPR *ex is the expression to be copied					**/
/** Returns: 													**/
/** 	a pointer to the copy of the original expression		**/
/****************************************************************/
Expression* Expression::clone()
{
	if(this == 0)
		return 0;

	Expression* clone = new Expression();

	*clone = *this;	/* copy across most data */
	clone->left = left->clone();  /* recreate left branch */
	clone->right = right->clone(); /* and right branch */
	return(clone);
}



void Expression::print_operator(enum e_operator op)
{
	switch (op)
		{
		case op_or: 	printf("|");	break;	
		case op_xor:	printf("^");	break;
		case op_and:	printf("&");	break;
		case op_not:	printf("~");	break;
		case op_eq:		printf("=");	break;
		case op_ne:		printf("<>");	break;
		case op_lt:		printf("<");	break;
		case op_gt:		printf(">");	break;
		case op_le:		printf("<=");	break;
		case op_ge:		printf(">=");	break;
		case op_plus:	printf("+");	break;
		case op_minus:	printf("-");	break;
		case op_mult:	printf("*");	break;
		case op_div:	printf("/");	break;
		case op_mod:	printf("%");	break;
		case op_shr:	printf(">>");	break;
		case op_shl:	printf("<<");	break;
		case op_negate:	printf("NEG");	break;
		case op_nop:	break;
		}
	return;
}

void Expression::print_expression()
{
	if (this == 0)
		return;
	
	switch (type)
		{
		case t_operator:
			putchar('(');
			left->print_expression();
			print_operator(data.op);
			right->print_expression();
			putchar(')');
			break;

		case t_number:
			printf("%d",data.value);
			break;

		case t_addr:
			printf("#%d(%s)",data.address.addr, data.address.segment->getName().c_str());
			break;

		case t_symbol:
			printf("%s",data.symbol->getName().c_str());
			break;
		}
	return;
}


