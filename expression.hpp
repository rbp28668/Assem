#ifndef EXPRESSION_H
#define EXPRESSION_H


/*************************************************************************/
/** EXPRESSIONS 														**/
/*************************************************************************/

class Symbol;
class Segment;

class Expression
{
public:
	struct AddressT {
		const Segment* segment;
		long addr;
	};

	enum e_expr_type {t_number,t_symbol,t_operator,t_addr};

	enum e_operator
	{
		op_or, 	
		op_xor,	
		op_and,	
		op_not,	
		op_eq,	
		op_ne,	
		op_lt,	
		op_gt,	
		op_le,	
		op_ge,	
		op_plus,	
		op_minus,	
		op_mult,	
		op_div,	
		op_mod,	
		op_shr,	
		op_shl,	
		op_negate,
		op_nop,
		op_last
	};

private:

	enum e_expr_type type;
	union
		{
			int value;
			Symbol *symbol;
			AddressT address; // a snapshot with a fixed address.
			enum e_operator op;
	} data;

	Expression *left;
	Expression *right;

	void print_operator(enum e_operator op);

	bool combine_numbers();
	bool combine_addresses();

public:
	Expression();
	Expression(Expression *l, enum e_operator op);
	Expression(Expression *l, Expression *r, enum e_operator op);

	~Expression();

	inline e_expr_type getType() const {return type;}

	// Helpers to pick apart the union
	inline int getValue() const {return data.value;}
	inline const Symbol* getSymbol() const {return data.symbol;}
	inline const AddressT getAddress() const {return data.address;}
	inline e_operator getOperator() const {return data.op;}

	inline Expression* getLeft() {return left;}
	inline Expression* getRight() {return right;}

	void resolve_symbols();
	bool resolve_expr(int& val);

	Expression* clone(); // was dup_expr()

	// Factory methods
	static Expression* ito_expr(int val);
	static Expression* addrto_expr(Segment* segment);
	static Expression* symto_expr(Symbol *sym);

	void minimise();

	void print_expression();
};


#endif