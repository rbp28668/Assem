/*
Title:				ALINK.C

Function:			Assembly Linker main program

Author:				Bruce

Date:				24 Feb 1991

Directory:

Calling sequence:

Input parameters:

Output parameters:

Description:

Non-library routines used:

Possible error codes returned:

Revision history:

@*/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <segdefs.h>
#include "assem.h"
#include "extern.h"
#include "error.h"

enum asm_cmd_type {asm_code,asm_data,asm_obj,asm_end,
			asm_cstart,asm_dstart,asm_binary,
			asm_intel};

KEYMATCH asm_cmd[]=
	{
	"code",asm_code,
	"data",asm_data,
	"obj",asm_obj,
	"cstart",asm_cstart,
	"dstart",asm_dstart,
	"end",asm_end,
	"intel",asm_intel,
	"binary,",asm_binary,
	NULL,-1
	};

int pass;
CURRENT_STATE *current;
int error_fd;			/* file descriptor for error text file */
int list_handle;		/* handle for list memory allocation */
int proc_handle;
FILE *codefp,*datafp;		/* file pointers for code and data files */

/*********************************************************/
/** PERFORM_PASS does one pass of the linker.		**/
/*********************************************************/
void perform_pass(void)
{
int cmd;
char *fn;
int start;
do
	{
	read_line();
	strip_comments(';');
	if (blank_line()) continue;
	
	cmd=get_keyword(asm_cmd);
	switch(cmd)
		{
		case asm_code:
			if(pass==2) break;
			if((fn=get_filename())==NULL)
				{
				error(NO_CODE_FILENAME);
				break;
				}
			if((codefp=fopen(fn,"w"))==NULL)
				{
				error(CODE_FILE_FAIL);
				break;
				}
			free(fn);
			break;
		case asm_data:
			if(pass==2) break;
			if((fn=get_filename())==NULL)
				{
				error(NO_DATA_FILENAME);
				break;
				}
			if((datafp=fopen(fn,"r"))==NULL)
				{
				error(DATA_FILE_FAIL);
				break;
				}
			free(fn);
			break;
		case asm_obj:
			if((fn=get_filename())==NULL)
				{
				error(NO_IP_FILENAME);
				break;
				}
			link_file(fn);
			free(fn);
			break;
		case asm_cstart:
			if(resolve_expr(get_expression(),&start))
				current->code_posn=start;
			else
				error(CANNOT_RESOLVE);
			break;
		case asm_dstart:
			if(resolve_expr(get_expression(),&start))
				current->data_posn=start;
			else
				error(CANNOT_RESOLVE);
			break;
		case asm_intel:
			break;
		case asm_binary:
			break;
		case asm_end:
			break;
		default:
			error(SYNTAX);
			break;
		}
	}
while(cmd!=asm_end);	
return;
}

/*********************************************************/
/** MAIN this is the main program of the LINKER		**/
/*********************************************************/
int main(int argc, char *argv[])
{

/* open up the error text file */
error_fd=open("english.err",O_RDONLY);
if(error_fd==-1)
	{
	fprintf(stderr,"Can't find english.err\n");
	return(-1);
	}

/* set up initial current state */
current=new(CURRENT_STATE);
reset_current_state(current);

/* Any input file ? */
if(argc<2)
	{
	error(NO_IP_FILENAME);
	free(current);
	return(1);
	}

if((current->infp=fopen(argv[1],"r"))==NULL)
	{
	error(NO_IP_FILE);
	return(1);
	}
current->infn=malloc(strlen(argv[1])+1);
strcpy(current->infn,argv[1]);

pass=1;
list_handle=block_create();
perform_pass();

/* if output files wern't opened during pass 1, open them with default */
/* file names */
if (codefp==NULL)
	{
	if((codefp=fopen("CODE.HEX","w"))==NULL)
		error(DUFF_OP_FILE);
	}
if (datafp==NULL)
	{
	if((datafp=fopen("DATA.HEX","w"))==NULL)
		error(DUFF_OP_FILE);
	}
	
reset_input();
current->code_posn=0L;
current->data_posn=0L;
current->line_no=0;

pass=2;
perform_pass();

dump_sym_tab(stdout);

set_intel_hex_file(codefp);
intel_hex_close();
set_intel_hex_file(datafp);
intel_hex_close();

free(current->infn);
block_free(list_handle);
fclose(current->infp);
free(current);
return(0);
}

	
	
	
	
