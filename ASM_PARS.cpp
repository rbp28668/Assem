/*
Title:				ASM_PARS.C

Function:			scan for and process assembler statements

Author:			Bruce

Date:				7 Dec 90

Directory:

Calling sequence:	asm_parse();

Input parameters:

Output parameters:	returns TRUE if valid assembler otherwise
					it returns FALSE.

Description:

Non-library routines used:

Possible error codes returned:

Revision history:

@*/
#include <stdio.h>
#include <stdlib.h>
#include "assem.h"
#include "extern.h"
#include "error.h"

extern OPLIST *opcode_list;
extern CURRENT_STATE *current;
extern int pass;

/*************************************************************************/
/** PARSE_SINGLE_ARG takes a parse list for a single argument and tries	**/
/** to parse it. If the parse is successfull the matching argument is	**/
/** taken from the input buffer and TRUE is returned. If the argument	**/
/** does not match then the input buffer is left unchanged and FALSE	**/
/** is returned.							**/
/*************************************************************************/
static int parse_single_arg(PLIST *arg)
{
char *linestart;
PLIST *here;
int value;
EXPR *ex;

linestart=current->lsptr;
for(here=arg;here!=NULL;here=here->next)
	{
	switch(here->type)
		{
		case text_type:
			if(!is_sym(here->item))
				{
				current->lsptr=linestart;
				return(FALSE);
				}
			break;
		case set_type:
			if(!find_set_member(&value,here->item))
				{
				current->lsptr=linestart;
				return(FALSE);
				}
			else
				push_prodn_stack(ito_expr(value));
			break;
		case expr_type:
			reset_error();
			ex=get_expression();
			if(check_error())
				{
				current->lsptr=linestart;
				destroy_expr(ex);
				return(FALSE);
				}
			else
				push_prodn_stack(ex);
		case plist_type:
			if (!parse_single_arg(here->item))
				return(FALSE);
			break;
		default:
			fatal("parse type");
			break;
		}
	}
return(TRUE);
}

/*************************************************************************/
/** PARSE_ASM_ARGS takes an argument list for a given opcode and tries	**/
/** to match one of the possible sets of arguments. If a match is found	**/
/** the corresponding production rules are run and TRUE is returned. 	**/
/** if none of the arguments match then FALSE is returned		**/
/*************************************************************************/
static int parse_asm_args(ARGLIST *args)
{
ARGLIST *here;

for(here=args;here!=NULL;here=here->next)
	{
	clear_prodn_stack();
	if(parse_single_arg(here->parse))
		{
		exe_prodn(here->prodn);
		return(TRUE);
		}
	}
return(FALSE);
}

/*************************************************************************/
/** ASM_PARSE tries to match an assembler opcode with is optional 	**/
/** arguments. If it finds a matching opcode then, if there are no	**/
/** arguments the production rules are run, otherwise it tries to match	**/
/** one of its possible sets of arguments.				**/
/*************************************************************************/
int asm_parse(void)
{
OPLIST *here;
for(here=opcode_list;here!=NULL;here=here->next)
	{
	if(is_sym(here->opcode))
		{
		if(here->args!=NULL)
			{
			if(!parse_asm_args(here->args))
				if(pass==2) error(SYNTAX);
			}
		else
			exe_prodn(here->prodn);
		return(TRUE);
		}
	}
return(FALSE);
}



