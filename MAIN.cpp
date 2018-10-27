/*
Title:			MAIN.C

Function:		Assembler main program

Author:			Bruce

Date:			7 December 1990

Directory:

Calling sequence:

Input parameters:

Output parameters:

Description:

Non-library routines used:

Possible error codes returned:

Revision history:

@*/

#pragma warning( disable: 4786)

#include "Exception.hpp"
#include "ProcessorDefinition.hpp"
#include "InputLine.hpp"
#include "AssemblyUnit.hpp"
#include "PDLParser.hpp"
#include "AssemblyGenerator.hpp"
#include "Error.h"

/*********************************************************/
/** MAIN						**/
/*********************************************************/

int main(int argc,char *argv[])
{

	// Default settings of params before processing args.
	char* proc = getenv("ASSEM");
	char* err = "english.err";
	bool generate = false;

	char** args = new char*[argc];
	int argCount = 0;

	for(int i=1;i<argc;++i)
	{
		if(strcmp(argv[i],"--proc") == 0)
		{
			++i;
			if(i < argc)
				proc = argv[i];
		} else if(strcmp(argv[i],"--err") == 0)
		{
			++i;
			if(i < argc)
				err = argv[i];
		} else if(strcmp(argv[i],"--generate") == 0) {
			generate = true;
		} else {
			args[argCount++] = argv[i];
		}

	}


	/* open up the error text file */
	Exception::setErrorFile(err);

	/* Open up  the processor description and read it */
	if(proc == 0)
		Exception::error(PDL_NOT_SPECIFIED);

	InputLine pdlInput(proc);
	PDLParser pdl(&pdlInput);
	ProcessorDefinition* processor = pdl.create();

	if(generate)
	{
		AssemblyGenerator gen;
		gen.write(processor);
	}

	/* Any input file ? */
	for(i=0;i<argCount;++i)		/* for all file names */
		{
		AssemblyUnit unit(processor);
		unit.process(args[i]);
		}

	delete args;
	delete processor;
	return 0;
}
