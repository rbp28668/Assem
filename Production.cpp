// Production.cpp: implementation of the Production class.
//
//////////////////////////////////////////////////////////////////////
#define TOS 	(pstack[pstkptr-1])		/* top of stack */
#define TOS2 	(pstack[pstkptr-2]) 	/* second top of stack */
#define NTOS(n)	(pstack[pstkptr-n]) 	/* nth top of stack */

#define PRODN_TRACE 0

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "Production.hpp"
#include "ProductionList.hpp"
#include "Exception.hpp"
#include "Plant.hpp"

Production::Production(int* _codes, int len)
: codes(0)
, length(len)
{
	assert(_codes);
	assert(len > 0);

	codes = new int[len];
	memcpy(codes, _codes, len*sizeof(int));

}

Production::~Production()
{
	delete [] codes;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProductionEngine::ProductionEngine(const ProductionList* list, Plant* plant)
{
	assert(plant != 0);
	assert(list != 0);

	pstkptr = 0;
	this->list = list;
	this->plant = plant;
}

ProductionEngine::~ProductionEngine()
{

}



/*****************************************************************/
/** PUSH sticks its integer onto the production stack		**/
/*****************************************************************/
void ProductionEngine::push(Expression* expr)
{
	assert(this);
	assert(expr);
	if(pstkptr>=PSTACKLEN)
		{
		Exception::error(PRODN_STACK_OVFLO);
		return;
		}
	pstack[pstkptr++]=expr;
}

/*****************************************************************/
/** POP pops an integer off the production stack		**/
/*****************************************************************/
Expression* ProductionEngine::pop()
{
	assert(this);
	if (pstkptr==0)
		{
		Exception::error(PRODN_STACK_UNDERFLO);
		return 0;
		}
	return(pstack[--pstkptr]);
}

/*****************************************************************/
/** CLEAR_PRODN_STACK allows external routines to empty the 	**/
/** production stack e.g. after a failed parse			**/
/*****************************************************************/
void ProductionEngine::clearStack()
{
	while(pstkptr > 0)
		{
		delete pstack[--pstkptr];
		}
	assert(pstkptr==0);
}


/****************************************************************/
/** COMBINE													**/
/** combines an operator with its arguments to produce a		**/
/** compound expression										**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
Expression* ProductionEngine::combine(Expression::e_operator op,Expression* left, Expression* right)
{
	return new Expression(left, right, op);

}

/*****************************************************************/
/** EXE_RPN executes a single production opcode			**/
/*****************************************************************/
void ProductionEngine::exe_rpn(int opcode)
{
int pickdepth;

switch(opcode)
	{
	case Production::dupl: push(TOS->clone()); break;	

	case Production::swap:
		if(pstkptr<2)
		 	Exception::error(PRODN_STACK_UNDERFLO);
		else
			{
			Expression* temp=TOS;
			TOS=TOS2;
			TOS2=temp;
			}
		break;

	case Production::over: 	push(TOS2->clone()); break;

	case Production::pick:
		{
			Expression* depth = pop();
			if(depth->resolve_expr(pickdepth))
				{
				push(NTOS(pickdepth)->clone());
				}
			delete depth;
		}
		break;

	case Production::drop: 	delete pop(); break;

	case Production::plus: 	push(combine(Expression::op_plus,pop(),pop()));		break;
	case Production::minus:	push(combine(Expression::op_minus,pop(),pop()));	break;
	case Production::shr:	push(combine(Expression::op_shr,pop(),pop()));		break;
	case Production::shl:	push(combine(Expression::op_shl,pop(),pop()));		break;
	case Production::not:	push(combine(Expression::op_not,pop(),0));			break;
	case Production::neg:	push(combine(Expression::op_negate,pop(),0)); 		break;
	case Production::xor:	push(combine(Expression::op_xor,pop(),pop()));		break;
	case Production::and:	push(combine(Expression::op_and,pop(),pop()));		break;
	case Production::or:	push(combine(Expression::op_or,pop(),pop()));		break;

	case Production::rel8:
			plant->plant_rel8(pop());
			break;

	case Production::rel16:
			plant->plant_rel16(pop());
			break;

	case Production::abs16:
			plant->plant_abs16(pop());
			break;

	case Production::byte:	plant->plant_expr_byte(pop()); break;
	case Production::word:	plant->plant_expr_word(pop()); break;
	case Production::pend:
	case Production::literal:	
	case Production::defn:
	default:
			Exception::fatal("Illegal production code");
			break;
	}
return;
}

/*****************************************************************/
#if PRODN_TRACE
void ProductionEngine::print_prodn_token(int tok)
{
	printf("%s ",PDLParser::lookupProduction(tok));
}

void ProductionEngine::dump_stack()
{
	int i;
	printf("( ");
	for(i=0;i<pstkptr;++i) printf("%d ",pstack[i]);
	printf(") ");
	return;
}

#else

void ProductionEngine::print_prodn_token(int tok) {}
void ProductionEngine::dump_stack() {}

#endif


/*****************************************************************/
/** SUB_EXE_PRODN executes the production list until the pend 	**/
/** opcode is found. The literal opcode is a special case- the	**/
/** following opcode is taken as literal data and pushed onto	**/
/** the stack rather than being executed			**/
/*****************************************************************/

void ProductionEngine::sub_exe_prodn(const Production& prodn)
{
	assert(this);

	for(const int* ip=prodn.getCodes(); *ip!=Production::pend; ++ip)
		{
		switch(*ip)
			{
			case Production::literal:
				++ip;
				push(Expression::ito_expr(*ip));
	#if PRODN_TRACE
				printf("%d ",*ip);
	#endif
				break;
			case Production::defn:
				{
					++ip;
					const Production* production = list->lookup(*ip);
					assert(production);
					sub_exe_prodn(*production); // TODO - see ProductionList.hpp
					break;
				}
			default:
				exe_rpn(*ip);
	#if PRODN_TRACE			
				print_prodn_token(*ip);
	#endif
				break;
			}
		}
	return;
}

/*****************************************************************/
/** EXE_PRODN							**/
/*****************************************************************/
void ProductionEngine::exe_prodn(const Production& prodn)
{
	#if PRODN_TRACE
	dump_stack();
	#endif

	sub_exe_prodn(prodn);	/* execute the production */

	#if PRODN_TRACE
	putchar('\n');
	#endif

	if(pstkptr!=0)
		{
		Exception::error(PRODN_STACK_NOT_EMPTY);
		clearStack();
		}
	return;
}
