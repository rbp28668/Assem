// Production.hpp: interface for the Production class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRODUCTION_HPP__BE80761C_F3FA_4365_9E61_8462422A0D58__INCLUDED_)
#define AFX_PRODUCTION_HPP__BE80761C_F3FA_4365_9E61_8462422A0D58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stack>
#include "expression.hpp"

class Plant;
class ProductionList;

class Production {
public:
	/** Keywords for production grammar **/
	enum prod_key {dupl,swap,over,pick,drop,plus,minus,shr,shl,not,neg,xor,and,
		or,rel8,rel16,abs16,byte,word,pend,literal,defn};

private:
	int* codes;
	int length;

public:
	Production(int* codes, int len);
	~Production();

	const int* getCodes() const {return codes;}

};

class ProductionEngine  
{

private:

	enum { PSTACKLEN = 32 };
	int pstkptr;
	Expression* pstack[PSTACKLEN];
	
	Plant* plant;			// where output goes to.
	const ProductionList* list;	// Keyed list of productions.

	Expression* pop();

	Expression* combine(Expression::e_operator op,Expression* left, Expression* right);
	void exe_rpn(int opcode);
	void sub_exe_prodn(const Production& prodn);

	// diagnostics
	void print_prodn_token(int tok);
	void dump_stack();


public:
	ProductionEngine(const ProductionList* list, Plant* plant);
	virtual ~ProductionEngine();

	void push(Expression* expr);
	void clearStack();
	void exe_prodn(const Production& prodn);
};

#endif // !defined(AFX_PRODUCTION_HPP__BE80761C_F3FA_4365_9E61_8462422A0D58__INCLUDED_)
