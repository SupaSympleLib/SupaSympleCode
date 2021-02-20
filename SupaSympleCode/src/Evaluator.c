#include "SupaSyC/Common.h"

typedef struct
{
	const AstNode *ast;
} Evaluator;

static const AstNode *Next(Evaluator *This)
{
	const AstNode *ast = This->ast;
	if (This->ast->Next)
		This->ast = This->ast->Next;
	return ast;
}

static int32_t EvaluateInternal(Evaluator *This)
{
	const AstNode *ast = Next(This);

	switch (ast->Kind)
	{
	case AST_Number:
	{
		const char *end = ast->Token->Text + ast->Token->Length;
		return strtol(ast->Token->Text, &end, 0);
	}
	case AST_Addition:
	{
		int32_t left = EvaluateInternal(This);
		int32_t right = EvaluateInternal(This);

		return left + right;
	}
	case AST_Subtraction:
	{
		int32_t left = EvaluateInternal(This);
		int32_t right = EvaluateInternal(This);

		return left - right;
	}
	case AST_Multiplication:
	{
		int32_t left = EvaluateInternal(This);
		int32_t right = EvaluateInternal(This);

		return left * right;
	}
	case AST_Division:
	{
		int32_t left = EvaluateInternal(This);
		int32_t right = EvaluateInternal(This);

		return left / right;
	}
	default:
		abort(); // Error
	}
}

int32_t Evaluate(const AstNode *ast)
{
	Evaluator *This = &(Evaluator) { ast };
	return EvaluateInternal(This);
}