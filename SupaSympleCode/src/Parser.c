#include "SupaSyC/Common.h"

typedef struct
{
	const Token *tok;
	AstNode *node;
} Parser;

static AstNode *NewParseAstNode(Parser *, AstKind, const Token *);
static AstNode *ParseExpression(Parser *);
static AstNode *ParseBinaryExpression(Parser *, uint8_t parentPrecedence);
static AstNode *ParsePrimaryExpression(Parser *);

static uint8_t GetBinaryOperatorPrecedence(const Token *);
static AstKind GetBinaryOperatorNode(const Token *);

static const Token *Next(Parser *);
static const Token *Match(Parser *, TokenKind kind);

AstNode *Parse(const Token *toks)
{
	AstNode defNode = (AstNode) { .Next = null };
	Parser *This = &(Parser) { toks, &defNode };

	while (This->tok->Kind != TK_EndOfFile)
	{
		This->node = ParseExpression(This);
		Match(This, TK_Semicolon);
	}

	if (defNode.Next)
		return defNode.Next;
	else
		return NewParseAstNode(This, AST_Null, toks);
}

static AstNode *ParseExpression(Parser *This)
{ return ParseBinaryExpression(This, 0); }

static AstNode *NewBinary(const Token *opTok, const AstNode *leftStart, AstNode *leftEnd, const AstNode *right)
{
	AstNode *ast = NewAstNode(GetBinaryOperatorNode(opTok), opTok, leftStart);
	leftEnd->Next = right;
	return ast;
}

static AstNode *ParseBinaryExpression(Parser *This, uint8_t parentPrecedence)
{
	AstNode *left = This->node->Next = ParsePrimaryExpression(This);
	AstNode *leftEnd = left;

	while (true)
	{
		const Token *opTok = This->tok;
		uint8_t precedence = GetBinaryOperatorPrecedence(opTok);
		if (!precedence || parentPrecedence && precedence >= parentPrecedence)
		{
			SetConsoleColor(ConsoleColor_DarkYellow);
			assert(!leftEnd->Next);
			return leftEnd;
		}
		Next(This);
		AstNode *right = ParseBinaryExpression(This, precedence);
		
		left = NewBinary(opTok, left, leftEnd, right);
		leftEnd = right;
	}
}

static AstNode *ParsePrimaryExpression(Parser *This)
{
	switch (This->tok->Kind)
	{
	case TK_Number:
		return NewParseAstNode(This, AST_Number, Next(This));
	default:
		ErrorAt(This->tok, "Illegal expression");
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
	ErrorAt(This->tok, "Unexpected %s '%.*s', expected %s", TokenKindNames[This->tok->Kind], This->tok->Length, This->tok->Text, TokenKindNames[kind]);
	return null;
}

static AstNode *NewParseAstNode(Parser *This, AstKind kind, const Token *tok)
{
	AstNode *node = NewAstNode(kind, tok, null);
	//This->node = This->node->Next = node;
	return node;
}

AstNode *NewAstNode(AstKind kind, const Token *tok, AstNode *next)
{
	AstNode *This = Alloc(1, AstNode);
	This->Kind = kind;
	This->Token = tok;
	This->Next = next;

	return This;
}

void DeleteAstNode(const AstNode *This, bool delNext)
{
	if (!This)
		return;

	DeleteToken(This->Token, false);
	if (delNext)
		DeleteAstNode(This->Next, delNext);
	Free(This);
}

void PrintAstNode(const AstNode *This)
{
	printf("%-18s -> ", AstKindNames[This->Kind]);
	PrintToken(This->Token);
}


static uint8_t GetBinaryOperatorPrecedence(const Token *tok)
{
	switch (tok->Kind)
	{
	case TK_Star:
	case TK_Slash:
	case TK_Percent:
		return 3;
	case TK_Plus:
	case TK_Minus:
		return 4;
	default:
		return 0;
	}
}

static AstKind GetBinaryOperatorNode(const Token *tok)
{
	switch (tok->Kind)
	{
	case TK_Plus:
		return AST_Addition;
	case TK_Minus:
		return AST_Subtraction;
	case TK_Star:
		return AST_Multiplication;
	case TK_Slash:
		return AST_Division;
	case TK_Percent:
		return AST_Modulo;
	default:
		ErrorAt(tok, "Internal error (Getting binary operator node on invalid token)");
		return AST_Null;
	}
}