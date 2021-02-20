#include "SupaSyC/Common.h"

typedef struct
{
	const Token *tok;
	AstNode *node;
} Parser;

static void NewParseAstNode(Parser *, AstKind, const Token *);
static void ParseExpression(Parser *);
static void ParseBinaryExpression(Parser *, uint8_t parentPrecedence);
static void ParsePrimaryExpression(Parser *);

static uint8_t GetBinaryOperatorPrecedence(TokenKind);
static AstKind GetBinaryOperatorNode(TokenKind);

static const Token *Next(Parser *);
static const Token *Match(Parser *, TokenKind kind);

AstNode *Parse(const Token *toks)
{
	AstNode defNode = (AstNode) { .Next = null };
	Parser *This = &(Parser) { toks, &defNode };

	ParseExpression(This);

	return defNode.Next;
}

static void ParseExpression(Parser *This)
{ ParseBinaryExpression(This, 0); }

static void ParseBinaryExpression(Parser *This, uint8_t parentPrecedence)
{
	AstNode *base = This->node;
	ParsePrimaryExpression(This);
	AstNode *left = This->node;

	while (true)
	{
		const Token *op = This->tok;
		uint8_t precedence = GetBinaryOperatorPrecedence(op->Kind);
		if (!precedence || parentPrecedence && precedence > parentPrecedence)
			break;
		NewParseAstNode(This, GetBinaryOperatorNode(op->Kind), Next(This));
		base->Next = This->node;
		This->node = This->node->Next = left;

		ParseBinaryExpression(This, precedence);
	}
}

static void ParsePrimaryExpression(Parser *This)
{
	switch (This->tok->Kind)
	{
	case TK_Number:
		NewParseAstNode(This, AST_Number, Next(This));
		break;
	}
}


static const Token *Next(Parser *This)
{
	const Token *tok = This->tok;
	if (tok->Next)
		This->tok = tok->Next;
	return tok;
}

static const Token *Match(Parser *This, TokenKind kind)
{
	if (This->tok->Kind == kind)
		return Next(This);
	return null;
}

static void NewParseAstNode(Parser *This, AstKind kind, const Token *tok)
{
	AstNode *node = NewAstNode(kind, tok, null);
	This->node = This->node->Next = node;
}

AstNode *NewAstNode(AstKind kind, const Token *tok, AstNode *next)
{
	AstNode *This = Alloc(1, AstNode);
	This->Kind = kind;
	This->Token = tok;
	This->Next = next;

	return This;
}

void DeleteAstNode(const AstNode *This, bool deleteNext)
{
	DeleteToken(This->Token, false);
	if (deleteNext)
		DeleteAstNode(This->Next, deleteNext);
	Free(This);
}

void PrintAstNode(const AstNode *This)
{
	printf("%-18s -> ", AstKindNames[This->Kind]);
	PrintToken(This->Token);
}


static uint8_t GetBinaryOperatorPrecedence(TokenKind kind)
{
	switch (kind)
	{
	case TK_Star:
	case TK_Slash:
		return 3;
	case TK_Plus:
	case TK_Minus:
		return 4;
	default:
		return 0;
	}
}

static AstKind GetBinaryOperatorNode(TokenKind kind)
{
	switch (kind)
	{
	case TK_Plus:
		return AST_Addition;
	case TK_Minus:
		return AST_Subtraction;
	case TK_Star:
		return AST_Multiplication;
	case TK_Slash:
		return AST_Division;
	default:
		return AST_Null;
	}
}