#include "SupaSyC/Common.h"

typedef struct
{
	const Token *tok;
	AstObject *obj;
	AstObject *objLast;
} Parser;

static AstObject *ParseMember(Parser *);
static AstObject *ParseFunctionDeclaration(Parser *);

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

AstObject *Parse(const Token *toks)
{
	AstObject obj = (AstObject) { .Next = null };
	Parser *this = &(Parser) { toks, &obj, &obj };

	while (this->tok->Kind != TK_EndOfFile)
	{
		this->objLast->Next = ParseMember(this);
		while (this->objLast->Next)
			this->objLast = this->objLast->Next;
	}
	
	if (obj.Next)
		return obj.Next;
	else
		return NewAstObject(false, true, toks, null, null);
}

static AstObject *ParseMember(Parser *this)
{
	switch (this->tok->Kind)
	{
	case TK_FuncKeyword:
		return ParseFunctionDeclaration(this);
	}
}

static AstObject *ParseFunctionDeclaration(Parser *this)
{
	Match(this, TK_FuncKeyword);
	const Token *name = Match(this, TK_Identifier);
	AstNode *body = ParseStatement(this, true);

	return NewAstObject(true, false, name, body, null);
}

static AstNode *ParseStatement(Parser *this, bool matchSemi)
{
	AstNode *stmt;
	switch (this->tok->Kind)
	{
	case TK_VarKeyword:
		stmt = ParseVariableDeclaration(this);
		break;
	default:
		stmt = ParseExpression(this);
		break;
	}

	if (matchSemi)
		Match(this, TK_Semicolon);
	return stmt;
}

static AstNode *ParseVariableDeclaration(Parser *this)
{
	Match(this, TK_VarKeyword);
	const Token *name = Match(this, TK_Identifier);
	
	AstObject *obj = NewAstObject(false, true, name, false, null);
	this->objLast = this->objLast->Next = obj;

	AstNode *node = NewAstNode(AST_VariableDeclaration, name, null);
	AstNode *init;
	if (this->tok->Kind == TK_Equal)
	{
		Next(this);
		init = ParseExpression(this);
	}
	else
		init = NewAstNode(AST_Null, name, null);
	node->Next = init;
	return node;
}


static AstNode *ParseExpression(Parser *this)
{ return ParseBinaryExpression(this, 0); }

static AstNode *ParseUnaryExpression(Parser *this, uint8_t parentPrecedence)
{
	const Token *opTok = this->tok;
	uint32_t precedence = GetUnaryOperatorPrecedence(opTok);
	if (precedence && (!parentPrecedence || precedence <= parentPrecedence))
	{
		Next(this); // Skip operator token
		AstNode *operand = ParseBinaryExpression(this, precedence);
		return NewUnaryAst(opTok, operand);
	}
	else
		return ParsePrimaryExpression(this);
}

static AstNode *ParseBinaryExpression(Parser *this, uint8_t parentPrecedence)
{
	AstNode *left = ParseUnaryExpression(this, parentPrecedence);

	while (true)
	{
		const Token *opTok = this->tok;
		uint8_t precedence = GetBinaryOperatorPrecedence(opTok);
		if (!precedence || parentPrecedence && precedence >= parentPrecedence)
			return left;
		Next(this); // Skip operator token
		AstNode *right = ParseUnaryExpression(this, precedence);
		
		left = NewBinaryAst(opTok, left, right);
	}
}

static AstNode *ParsePrimaryExpression(Parser *this)
{
	switch (this->tok->Kind)
	{
	case TK_Number:
		return NewAstNode(AST_Number, Next(this), null);
	default:
		ErrorAt(this->tok, "Expected expression");
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


static const Token *Next(Parser *this)
{
	const Token *tok = this->tok;
	if (tok->Next)
		this->tok = tok->Next;
	return tok;
}

static const Token *Match(Parser *this, TokenKind kind)
{
	if (this->tok->Kind == kind)
		return Next(this);
	ErrorAt(this->tok, "Unexpected %s '%.*s', expected %s", TokenKindNames[this->tok->Kind], this->tok->Length, this->tok->Text, TokenKindNames[kind]);
	return null;
}

AstNode *NewAstNode(AstKind kind, const Token *tok, AstNode *next)
{
	AstNode *this = Alloc(1, AstNode);
	this->Kind = kind;
	this->Token = tok;
	this->Next = next;

	return this;
}


AstObject *NewAstObject(bool isFn, bool isVar, const Token *tok, const AstNode *body, AstObject *next)
{
	AstObject *this = Alloc(1, AstObject);
	this->IsFunction = isFn;
	this->IsVariable = isVar;
	this->Token = tok;
	this->Body = body;
	this->Next = next;

	return this;
}


void PrintAstNode(const AstNode *this)
{
	printf("%-24s -> ", AstKindNames[this->Kind]);
	PrintToken(this->Token);
}

void PrintAstObject(const AstObject *this)
{
	printf("%s %.*s -> ", this->IsFunction ? "func" : "var ", this->Token->Length, this->Token->Text);
	PrintToken(this->Token);
	if (this->IsFunction)
	{
		const AstNode *node = this->Body;
		assert(node);
		do
		{
			putchar('\t');
			SetConsoleColor(ConsoleColor_Cyan);
			printf("%s", node->Next ? "|--" : "L--");
			SetConsoleColor(ConsoleColor_Green);
			PrintAstNode(node);
		}
		while (node = node->Next);
	}
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