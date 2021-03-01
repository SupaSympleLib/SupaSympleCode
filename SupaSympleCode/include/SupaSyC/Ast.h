#pragma once

typedef enum
{
	AST_Null,

	// Statements
	AST_Block,
	AST_EndBlock,
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

	AST_Call,

	// Flags
	AST_Statement = AST_Block, // Start of statements
	AST_Expression = AST_Positive, // Start of expressions
	AST_ExpressionLast = AST_Call, // Start of expressions
	AST_Last = AST_Call,
} AstKind;

static const char *const AstKindNames[AST_Last + 1] =
{
	"NullAst",


	"BlockAst",
	"EndBlockAst",
	"VarDeclAst",


	"PositiveAst",
	"NegativeAst",

	"AdditionAst",
	"SubtractionAst",
	"MultiplicationAst",
	"DivisionAst",
	"ModuloAst",

	"NumberAst",

	"CallAst",
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

	const AstNode *Body; // Function Body

	AstObject *Next;
};

AstNode *NewAstNode(AstKind, const Token *token, AstNode* next);
AstObject *NewAstObject(bool isFunction, bool isVariable, const Token *token, const AstNode *body, AstObject *next);

bool AstIsExpr(const AstNode *);

void PrintAstNode(const AstNode *);
void PrintAstObject(const AstObject *);