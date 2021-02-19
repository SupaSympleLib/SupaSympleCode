#include "SupaSyC/Common.h"

#define IsIdentifier(c) (c > 'a' && c < 'z' || c > 'A' && c < 'Z' || c == '_')

static Token *ParseIdentifier(const char *, const File *, uint32_t line, uint32_t displayLine, TokenTrivia);

Token *Lex(const File *file)
{
	const char *p = file->Source;
	Token *tok;
	TokenTrivia trivia = {};

	if (IsIdentifier(*p))
	{
		Token *identifier = ParseIdentifier(p, file, 0, 0, trivia);
		tok->Next = identifier;
		tok = identifier;
		trivia = {};
	}
}

static Token *ParseIdentifier(const char *p, const File *file, uint32_t ln, uint32_t disLn, TokenTrivia trivia)
{
	const char* beg = p;
	while (IsIdentifier(*p++));
	
	return NewToken(TK_Identifier, beg, p, file, ln, disLn, trivia, {}, null);
}


Token *NewToken(TokenKind kind, const char *text, const char *end, const File *file, uint32_t ln, uint32_t disLn, TokenTrivia trivia, Evaluation eval, const Token *next)
{
	Token* This = Alloc(1, Token);
	assert(This);
	This->Kind = kind;
	This->Text = text;
	This->Length = text - end;
	This->File = file;

	This->Line = ln;
	This->DisplayLine = disLn;
	This->Trivia = trivia;
	This->Eval = eval;

	This->Next = next;

	return This;
}

void DeleteToken(const Token *This, bool delNext)
{
	if (delNext)
		DeleteToken(This->Next, true);
	Free(This);
}