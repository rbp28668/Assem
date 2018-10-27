#ifndef _PLANT_H_INC
#define _PLANT_H_INC

#include <stdio.h>
#include "Listing.hpp"
#include "Expression.hpp"
#include "SegmentList.h"
#include "InputLine.hpp"

class Symbol;
class Segment;
class SymbolTable;

class Plant : public InputLineListener {

public:
	typedef unsigned short ADDRESS;

private:
	int pass;
	FILE* outfp;
	static char xdigits[];

	Listing listing;
	Segment* currentSegment;
	SegmentList segments;
	bool littleEndian;

	void stuff_byte(int value);
	void bump(int nbytes);

	void write_byte(int value);
	void write_word(ADDRESS word);
	void plant_ascii(const char *str);
	void setCurrentSegment(Segment* segment);

public:

	Plant();
	~Plant();

	void plant_byte(int value);
	void plant_word(ADDRESS word);
	void plant_string(const char *str);
	void plant_type(const Segment& seg);
	void plant_addr(ADDRESS addr);
	void plant_comment(const char *string);
	void plant_public(const Segment& seg, ADDRESS addr,const char *name);
	void plant_org(ADDRESS addr);
	void plant_reloc(void);
	void plant_phase(ADDRESS phase);
	void plant_endian(void);
	void plant_defs(int size);
	void plant_expr_byte(Expression *ex);
	void plant_expr_word(Expression *ex);
	void plant_rel8(Expression *ex);
	void plant_rel16(Expression *ex);
	void plant_abs16(Expression *ex);
	void plant_segment(const char *name);

	Segment& getCurrentSegment();
	void resetSegments();

	void write_public(SymbolTable* symbols);

	void openOutput(const char* path);

	void setPass(int pass);

	// listing handling
	virtual void onNewLine(const char* line, int lineNo);
	void openListing(const char* path);
	void listSymbolTable(const SymbolTable* symbols);

	// In PlantExpr.cpp
private:
	void plant_st_entry(const Symbol *st);
	void plant_operator(Expression::e_operator op);
public:
	void plant_expr(Expression *ex);
};

#endif


