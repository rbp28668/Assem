// Exception.hpp: interface for the Exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EXCEPTION_HPP_INCLUDED_)
#define EXCEPTION_HPP_INCLUDED_

#include <exception>

class InputLine;

class Exception : public std::exception  
{
	static int error_fd;

	char** text;
	char** get_text(int fd,int where);


public:
	static void setErrorFile(const char* path);
	Exception(int err);
	virtual ~Exception();
    virtual const char *what() const throw();

	static void error(int err);
	static void error(int err, InputLine* line);
	static void fatal(char* str);
};

#endif // !defined(AFX_EXCEPTION_HPP__16C5949F_F31C_47FA_B501_9E2270D29E27__INCLUDED_)
