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

#define BIN_OP(op)                              \
	{                                           \
		int32_t left = EvaluateInternal(This);  \
		int32_t right = EvaluateInternal(This); \
		                                        \
		return left op right;                   \
	}                                           \

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
		BIN_OP(+);
	case AST_Subtraction:
		BIN_OP(-);
	case AST_Multiplication:
		BIN_OP(*);
	case AST_Division:
		BIN_OP(/ );
	case AST_Modulo:
		BIN_OP(%);
	case AST_Null:
		return null;
	default:
		ErrorAt(ast->Token, "Expected expression");
	}
}

int32_t Evaluate(const AstNode *ast, AstNode **astPtr)
{
	Evaluator *This = &(Evaluator) { ast };
	int32_t eval = EvaluateInternal(This);
	if (astPtr)
		*astPtr = This->ast;
	else
		DeleteAstNode(ast, true);
	return eval;
}