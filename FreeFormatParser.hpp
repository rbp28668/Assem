// FreeFormatParser.hpp: interface for the FreeFormatParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FREEFORMATPARSER_HPP_INCLUDED
#define FREEFORMATPARSER_HPP_INCLUDED

class InputLine;
class Plant;

class FreeFormatParser  
{
	Plant* plant;
	InputLine* line;

	static char ffmtstart[];
	void do_free_fmt_bit(int pass);

public:
	FreeFormatParser(InputLine* input, Plant* plant);
	virtual ~FreeFormatParser();

	bool parse(int pass);
};

#endif 
