#ifndef _LISTING_H_INC
#define _LISTING_H_INC

#define LINELEN 80

class Segment;
class SymbolTable;

class Listing {

	int pass;
	char list_buffer[LINELEN+1];
	int objcount;
	bool enabled;
	FILE* listfp;

public:

	Listing();
	~Listing();

	void create_line(const char* source, int line, const Segment& seg);
	void flush_line();
	void add_byte(int byte);

	void appendSymbolTable(const SymbolTable* symbols);

	void setPass(int pass);
	void openOutput(const char* path);
};

#endif