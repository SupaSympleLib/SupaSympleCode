#pragma once

typedef enum
{
	AST_Null,

	AST_Positive, // Unused I suppose...
	AST_Negative,

	AST_Addition,
	AST_Subtraction,
	AST_Multiplication,
	AST_Division,
	AST_Modulo,

	AST_Number,

	AST_Last = AST_Number,
} AstKind;

static const char *const AstKindNames[AST_Last + 1] =
{
	"NullAst",

	"PositiveAst",
	"NegativeAst",

	"AdditionAst",
	"SubtractionAst",
	"MultiplicationAst",
	"DivisionAst",
	"ModuloAst",

	"NumberAst",
};


typedef struct AstNode AstNode;
struct AstNode
{
	AstKind Kind;
	const Token *Token;

	AstNode *Next;
};

AstNode *NewAstNode(AstKind, const Token *token, AstNode* next);
void DeleteAstNode(const AstNode *, bool deleteNext);

void PrintAstNode(const AstNode *);