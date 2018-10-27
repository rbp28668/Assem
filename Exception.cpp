// Exception.cpp: implementation of the Exception class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "Exception.hpp"
#include "InputLine.hpp"


// File descripter of error file.
int Exception::error_fd = -1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Exception::Exception( int err )
: text(0)
{
	if(error_fd != -1)
	{
		text = get_text(error_fd, err);
	}
	else
	{
		text = new char*[2];
		text[0] = new char[16];
		text[1] = 0;

		sprintf(text[0],"%d",err);
	}
}

Exception::~Exception()
{
	delete text;
}

void Exception::setErrorFile(const char* path){
	error_fd = open(path,O_RDONLY);
	if(error_fd == -1)
		{
		fprintf(stderr,"Can't find %s\n", path);
		}
}

const char* Exception::what() const throw(){
	return *text;
}

char** Exception::get_text(int fd,int where)
{
	long sizes[2];

	/*first get the start and finish of the text */
	long fpos = where*sizeof(long);
	if (fpos!=lseek(fd,fpos,0)) return(0);
	if (2*sizeof(long) != read(fd,(char*)sizes,2*sizeof(long))) return(0);
	int length=(int)(sizes[1]-sizes[0]);

	/*now get enough store to hold a temp copy of the strings*/
	char* tstore = new char[length];

	/*and read the text into memory */
	if(sizes[0] != lseek(fd,sizes[0],0)) {delete [] tstore; return(0);}
	if(length != read(fd,tstore,length)) {delete [] tstore; return(0);}

	/*count the number of strings read in */
	int nstrings=0;
	char* here = tstore;
	while(here<(tstore+length))
		{
		++nstrings;
		here += (*here)+1;
		}

	/* get enough memory to hold the strings & their pointers */
	char* start = new char [sizeof(long)*(nstrings+1)+length];

	/* and copy them across while setting up pointers to them */
	char** vect=(char**) start;						/* where the pointers go */
	char* there = start+sizeof(long)*(nstrings+1);		/* where the text goes */
	here=tstore;								/* where the text comes from */
	while (nstrings--)
		{
		strncpy(there,here+1,*here);			/* copy a string across */
		*vect++ =there;							/* set a pointer to it */
		there+=*here;							/* skip over string */
		*there++ ='\0';							/* and terminate it */
		here+= *here +1;						/* pt to next string to copy */
		}

	/* now terminate list of pointers with a NULL */
	*vect=0;

	delete[] tstore;
	return((char**) start);
}

void Exception::error(int err)
{
	Exception exception(err);
	printf("ERROR: %s", exception.what());
}

void Exception::error(int err, InputLine* line)
{
	Exception exception(err);
	if(line == 0)
		printf("ERROR: %s\n", exception.what());
	else
		printf("ERROR: %s in %s, line %d\n", 
		exception.what(), 
		line->getSourcePath().c_str(), 
		line->getLineNumber());

}

void Exception::fatal(char* str)
{
	assert(str);

	printf("FATAL: %s\n", str);
	exit(1);
}
