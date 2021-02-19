#pragma once

typedef enum
{
	AST_Plus,
	AST_Number,

	AST_Last = AST_Number,
} AstKind;

static const char *const AstKindNames[AST_Last + 1] =
{
	"Plus",
	"Number",
};


typedef struct AstNode AstNode;
struct AstNode
{
	AstKind Kind;
	const Token *Token;

	AstNode *Next;
};