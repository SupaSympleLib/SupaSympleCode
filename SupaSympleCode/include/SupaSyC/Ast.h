#pragma once

typedef enum
{
	AST_Null,

	// Statements
	AST_VariableDeclaration,

	// Expressions / Operators
	AST_Positive, // Unused I suppose...
	AST_Negative,

	AST_Addition,
	AST_Subtraction,
	AST_Multiplication,
	AST_Division,
	AST_Modulo,

	AST_Number,

	// Flags
	AST_Statement = AST_VariableDeclaration, // Start of statements
	AST_Expression = AST_Positive,           // Start of expressions
	AST_Last = AST_Number,
} AstKind;

static const char *const AstKindNames[AST_Last + 1] =
{
	"NullAst",


	"VariableDeclarationAst",


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

typedef struct AstObject AstObject;
struct AstObject
{
	bool IsFunction : 1;
	bool IsVariable : 1;
	const Token *Token;

	AstObject *Next;
};

AstNode *NewAstNode(AstKind, const Token *token, AstNode* next);
AstObject *NewAstObject(bool isFunction, bool isVariable, const Token *token, AstObject *next);

void PrintAstNode(const AstNode *);
void PrintAstObject(const AstObject *);