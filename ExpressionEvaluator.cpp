#include <assert.h>
#include "ExpressionEvaluator.hpp"
#include "Expression.hpp"
#include "Exception.hpp"
#include "InputLine.hpp"
#include "Error.h"

/** PRODUCE takes a string, the recursion function name and the operator **/
/** name and expands the macro to check for the string and call the other **/
/** two bits and return if the string matched **/
#define produce(s,f,a) if(line->is_sym(s)){f(); a; return;}

/** OPERATOR MACROS **/

#define NORMAL	1


#if NORMAL
#define OR 	push(binary_op(pop(),pop(),Expression::op_or))
#define XOR	push(binary_op(pop(),pop(),Expression::op_xor))
#define AND	push(binary_op(pop(),pop(),Expression::op_and))
#define NOT	push(unary_op(pop(),Expression::op_not))
#define EQ		push(binary_op(pop(),pop(),Expression::op_eq))
#define NE		push(binary_op(pop(),pop(),Expression::op_ne))
#define LT		push(binary_op(pop(),pop(),Expression::op_lt))
#define GT		push(binary_op(pop(),pop(),Expression::op_gt))
#define LE		push(binary_op(pop(),pop(),Expression::op_le))
#define GE		push(binary_op(pop(),pop(),Expression::op_ge))
#define PLUS	push(binary_op(pop(),pop(),Expression::op_plus))
#define MINUS	push(binary_op(pop(),pop(),Expression::op_minus))
#define MULT	push(binary_op(pop(),pop(),Expression::op_mult))
#define DIV	push(binary_op(pop(),pop(),Expression::op_div))
#define MOD	push(binary_op(pop(),pop(),Expression::op_mod))
#define SHR	push(binary_op(pop(),pop(),Expression::op_shr))
#define SHL	push(binary_op(pop(),pop(),Expression::op_shl))
#define NEGATE	push(unary_op(pop(),Expression::op_negate))
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

/****************************************************************/
/****************************************************************/
ExpressionEvaluator::ExpressionEvaluator(InputLine* il, int _pass)
: line(il)
, pass(_pass)
{
	assert(il);
	expression(); // -- go parse.
}

ExpressionEvaluator::~ExpressionEvaluator()
{
	while(!eval_stack.empty())
	{
		delete pop();
	}
}

/****************************************************************/
/** PUSH														**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
void ExpressionEvaluator::push(Expression *ex)
{
	assert(this);
	assert(ex != 0);
	eval_stack.push(ex);
}

/****************************************************************/
/** POP														**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* ExpressionEvaluator::pop()
{
	assert(this);
	if(eval_stack.empty())
	{
		Exception::error(ILL_FORMED_EXPR, line);
		return(0);
	}
	Expression* ex = eval_stack.top();
	eval_stack.pop();
	return ex;
}

/****************************************************************/
/** UNARY_OP													**/
/** takes a unary operator and its operand and combines them	**/
/** into a compound expression									**/
/** Parameters:													**/
/** Returns: 													**/
/****************************************************************/
Expression* ExpressionEvaluator::unary_op(Expression* l, Expression::e_operator op)
{
	assert(l);
	Expression* ex = new Expression(l, op);
	return ex;
}

/****************************************************************/
/** BINARY_OP													**/
/** takes 2 operands and a binary operator and makes a compound**/
/** expression from them										**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* ExpressionEvaluator::binary_op(Expression* l, Expression* r, Expression::e_operator op)
{
	assert(l);
	assert(r);

	Expression* ex = new Expression(l, r, op);
	return ex;
}



/*************************************************************************/
/** EXPRESSION_RESULT returns the result of the most recent expression	**/
/*************************************************************************/
Expression* ExpressionEvaluator::result()
{
	assert(this);
	#if NORMAL
	return(pop());
	#else
	putchar('\n');
	return 0;
	#endif
}

/*************************************************************************/
/** EXPRESSION parses the input buffer and evaluates an expression.	**/
/*************************************************************************/
void ExpressionEvaluator::expression()
{
	assert(this);

	and_expr();
	produce("OR",expression,OR);
	produce("XOR",expression,XOR);
	return;
}

void ExpressionEvaluator::and_expr()
{
	assert(this);

	cond_expr();
	produce("AND",and_expr,AND);
	return;
}

void ExpressionEvaluator::cond_expr()
{
	assert(this);

	arith_expr();
	produce("=" ,cond_expr,EQ);
	produce("EQ",cond_expr,EQ);
	produce("<>",cond_expr,NE);
	produce("NE",cond_expr,NE);
	produce(">=",cond_expr,GE);
	produce("GE",cond_expr,GE);
	produce("<=",cond_expr,LE);
	produce("LE",cond_expr,LE);
	produce(">" ,cond_expr,GT);
	produce("GT",cond_expr,GT);
	produce("<" ,cond_expr,LT);
	produce("LT",cond_expr,LT);
	return;
}

void ExpressionEvaluator::arith_expr()
{
	assert(this);

	term();
	produce("+",arith_expr,PLUS);
	produce("-",arith_expr,MINUS);
	return;
}

void ExpressionEvaluator::term()
{
	assert(this);

	factor();
	produce("*",term,MULT);
	produce("/",term,DIV);
	produce("MOD",term,MOD);
	produce("SHR",term,SHR);
	produce(">>",term,SHR);
	produce("SHL",term,SHL);
	produce("<<",term,SHL);
	return;
}

void ExpressionEvaluator::factor()
{
	assert(this);

	produce("+",factor,NO_OP);
	produce("-",factor,NEGATE);
	produce("NOT",factor,NOT);
	number();
	return;
}

void ExpressionEvaluator::number()
{
	assert(this);

	if(line->is_sym("("))
	{
		expression();
		if(!line->is_sym(")") && (pass==1)) 
		{
			Exception::error(PARENTHESES, line);
			return;
		}
	}

	#if NORMAL
	push(line->get_number());
	#else
	printf("%d ",line->get_number());
	#endif
	return;
}

