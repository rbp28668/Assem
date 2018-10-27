#ifndef INPUT_LINE_H
#define INPUT_LINE_H

#define LINEBUFFSIZE 256	/* size of input line buffer */

#include <string>
#include <list>

class Expression;
class Segment;
class SymbolTable;

struct KEYMATCH
	{
	char *name;
	int key;
	};

// Class to allow others to register as listeners so they can 
// act on content begin read in (such as updating a listing file).
struct InputLineListener{
	virtual void onNewLine(const char* line, int lineNo) = 0;
};

class InputLine {

	enum st {initial,instring,escape,caret,finish};

	static char duff_fcs[]; /* chars not allowed in a fn. */
	static char numsyms[];  // chars allowed in a number.

	std::string source_path;	/* name of file */
	FILE		*infp;			/* input file pointer */
	long		start_of_line;	/* pointer into file*/
	int			line_no;		/* in current file */
	bool		eof_flag;

	char		linebuff[LINEBUFFSIZE];
	char		*lsptr;			/* start of line in buffer */

	char *bstart;
	char *buff;
	
	int radix;					/* current default radix */
	int pass;					// pass of assembler - controls error reporting
	Segment* currentSegment;	// current memory segment - for $ operator.
	SymbolTable* symbolTable;	// symbol table of current scope.

	typedef std::list<InputLineListener*> ListenersT;
	ListenersT listeners;

	inline bool istok(char c) { return((c!='\0')&&(isalnum(c)||(c == '.')));}
	inline bool isfilechar(char c) {return(isprint(c)&&(strchr(duff_fcs,c)==NULL));}
	inline bool isnumsym(char c) {return((c!='\0')&&(strchr(numsyms,c)!=NULL));}
	inline char xlat(char c) { return(isdigit(c)?(c-'0'):(toupper(c)-'A'+10));}

	std::string ename;
	inline std::string& extern_name() {return ename;}
	inline void log_extern_name(const std::string& name) {ename = name;  }

	void fireOnNewLine();

public:
	InputLine(const char* path);
	~InputLine();

	void setPass(int pass);
	void setSegment(Segment * seg);
	void setSymbolTable(SymbolTable* table);

	bool is_sym(const char *sym);
	bool blank_line();
	void strip_comments(char cchar);
	void strip_leading(char ch);
	void skip_space();
	void check_line();

	char* get_name();
	char *get_string();
	char *get_filename();
	int get_int();
	Expression *get_number();
	Expression *get_expression();

	// Allow char by char reading of buffer
	char current() {return *lsptr;}
	void skip() {++lsptr;}

	// Allow reverting to previous state on line if partial parse fails.
	char* getMark() {return lsptr;}
	void revertToMark(char* mark) {lsptr = mark;}

	int get_keyword(KEYMATCH keys[]);
	char* skipLabel();
	void save_buffer();
	void free_buffer();

	bool read_line();
	void reset_input();

	int getLineNumber() const {return line_no;}
	const std::string& getSourcePath() const {return source_path;}
	bool isEOF() const {return eof_flag;}

	void registerListener(InputLineListener* listener);
	void removeListener(InputLineListener* listener);
};

#endif