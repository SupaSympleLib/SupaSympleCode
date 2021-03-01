#include "SupaSyC/Common.h"

typedef struct
{
	const AstNode *ast;
} Evaluator;

static const AstNode *Next(Evaluator *this)
{
	const AstNode *ast = this->ast;
	if (this->ast->Next)
		this->ast = this->ast->Next;
	return ast;
}

#define UN_OP(op)                                 \
	{                                             \
		int32_t operand = EvaluateInternal(this); \
		                                          \
		return op operand;                        \
	}                                             \

#define BIN_OP(op)                                \
	{                                             \
		int32_t left = EvaluateInternal(this);    \
		int32_t right = EvaluateInternal(this);   \
		                                          \
		return left op right;                     \
	}                                             \

static int32_t EvaluateInternal(Evaluator *this)
{
	const AstNode *ast = Next(this);

	switch (ast->Kind)
	{
	case AST_Number:
	{
		const char *end = ast->Token->Text + ast->Token->Length;
		return strtol(ast->Token->Text, &end, 0);
	}
	case AST_Positive:
		UN_OP(+);
	case AST_Negative:
		UN_OP(-);
	case AST_Addition:
		BIN_OP(+);
	case AST_Subtraction:
		BIN_OP(-);
	case AST_Multiplication:
		BIN_OP(*);
	case AST_Division:
		BIN_OP(/ );
	case AST_Modulo:
		BIN_OP(%);
	case AST_VariableDeclaration:
		return EvaluateInternal(this); // Return the initializer
	case AST_Null:
		return null;
	default:
		ErrorAt(ast->Token, "Expected expression");
	}
}

int32_t Evaluate(const AstNode *ast, AstNode **astPtr)
{
	Evaluator *this = &(Evaluator) { ast };
	int32_t eval = EvaluateInternal(this);
	if (astPtr)
		*astPtr = this->ast;
	return eval;
}