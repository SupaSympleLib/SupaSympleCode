#include "SupaSyC/Common.h"

typedef struct
{
	const Token *tok;
	AstNode *node;
} Parser;

static AstNode *ParseStatement(Parser *, bool matchSemicolon);
static AstNode *ParseVariableDeclaration(Parser *);

static AstNode *ParseExpression(Parser *);
static AstNode *ParseUnaryExpression(Parser *, uint8_t parentPrecedence);
static AstNode *ParseBinaryExpression(Parser *, uint8_t parentPrecedence);
static AstNode *ParsePrimaryExpression(Parser *);

static AstNode *NewUnaryAst(const Token *opertorToken, AstNode *operand);
static AstNode *NewBinaryAst(const Token *opertorToken, AstNode *left, const AstNode *right);

static uint8_t GetUnaryOperatorPrecedence(const Token *);
static AstKind GetUnaryOperatorNode(const Token *);
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
		This->node->Next = ParseStatement(This, true);
		while (This->node->Next)
			This->node = This->node->Next;
	}

	if (defNode.Next)
		return defNode.Next;
	else
		return NewAstNode(AST_Null, toks, null);
}


static AstNode *ParseStatement(Parser *This, bool matchSemi)
{
	AstNode *stmt;
	switch (This->tok->Kind)
	{
	case TK_VarKeyword:
		stmt = ParseVariableDeclaration(This);
		break;
	default:
		stmt = ParseExpression(This);
		break;
	}

	if (matchSemi)
		Match(This, TK_Semicolon);
	return stmt;
}

static AstNode *ParseVariableDeclaration(Parser *This)
{
	Match(This, TK_VarKeyword);
	const Token *name = Match(This, TK_Identifier);
	
	return NewAstNode(AST_VariableDeclaration, name, null);
}


static AstNode *ParseExpression(Parser *This)
{ return ParseBinaryExpression(This, 0); }

static AstNode *ParseUnaryExpression(Parser *This, uint8_t parentPrecedence)
{
	const Token *opTok = This->tok;
	uint32_t precedence = GetUnaryOperatorPrecedence(opTok);
	if (precedence && (!parentPrecedence || precedence <= parentPrecedence))
	{
		Next(This); // Skip operator token
		AstNode *operand = ParseBinaryExpression(This, precedence);
		return NewUnaryAst(opTok, operand);
	}
	else
		return ParsePrimaryExpression(This);
}

static AstNode *ParseBinaryExpression(Parser *This, uint8_t parentPrecedence)
{
	AstNode *left = ParseUnaryExpression(This, parentPrecedence);

	while (true)
	{
		const Token *opTok = This->tok;
		uint8_t precedence = GetBinaryOperatorPrecedence(opTok);
		if (!precedence || parentPrecedence && precedence >= parentPrecedence)
			return left;
		Next(This); // Skip operator token
		AstNode *right = ParseUnaryExpression(This, precedence);
		
		left = NewBinaryAst(opTok, left, right);
	}
}

static AstNode *ParsePrimaryExpression(Parser *This)
{
	switch (This->tok->Kind)
	{
	case TK_Number:
		return NewAstNode(AST_Number, Next(This), null);
	default:
		ErrorAt(This->tok, "Illegal expression");
	}
}


static AstNode *NewUnaryAst(const Token *opTok, AstNode *operand)
{ return NewAstNode(GetUnaryOperatorNode(opTok), opTok, operand); }

static AstNode *NewBinaryAst(const Token *opTok, AstNode *left, const AstNode *right)
{
	AstNode *ast = NewAstNode(GetBinaryOperatorNode(opTok), opTok, left);
	while (left->Next)
		left = left->Next;
	left->Next = right;
	return ast;
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


static uint8_t GetUnaryOperatorPrecedence(const Token *tok)
{
	switch (tok->Kind)
	{
	case TK_Plus:
	case TK_Minus:
		return 2;
	default:
		return 0;
	}
}

static AstKind GetUnaryOperatorNode(const Token *tok)
{
	switch (tok->Kind)
	{
	case TK_Plus:
		return AST_Positive;
	case TK_Minus:
		return AST_Negative;
	default:
		ErrorAt(tok, "Internal error (Getting unary operator node on invalid token)");
		return AST_Null;
	}
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