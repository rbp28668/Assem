#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include <stack>
#include "Expression.hpp"

class InputLine;


class ExpressionEvaluator {

	std::stack<Expression*> eval_stack;

//	enum asm_scope this_expr_scope;
//	enum asm_type this_expr_type;

	InputLine* line;
	int pass;

	Expression* pop();
	void push(Expression *ex);

	/* expr evaluation */
	void expression();
	void and_expr();
	void cond_expr();
	void arith_expr();
	void term();
	void factor();
	void number();

	static Expression* unary_op(Expression* l, Expression::e_operator op);
	static Expression* binary_op(Expression* l, Expression* r, Expression::e_operator op);

public:

	ExpressionEvaluator(InputLine* input, int pass);
	~ExpressionEvaluator();

	// get result of evaluation.
	Expression* result();

};


#endif