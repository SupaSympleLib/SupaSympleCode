#include "SupaSyC/Common.h"

typedef struct
{
	const char *p;
	const File *file;
	Token *tok;
	TokenTrivia triv;

	uint32_t ln;
	uint32_t col;
	uint32_t disLn;
} Lexer;

static const char *Next(Lexer *);
static void ParseIdentifier(Lexer *);
static void NewLexToken(Lexer *This, TokenKind kind, const char *beg, Evaluation eval);

static bool IsIdentifier(char c)
{ return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_'; }
static bool IsWhiteSpace(char c)
{ return c == ' ' || c == '\t' || c == '\r' || c == 0xCC; }

Token *Lex(const File *file)
{
	Token defTok;
	Lexer lexObj = (Lexer) { file->Source, file, &defTok, (TokenTrivia) { false, false }, 0, 0 };
	Lexer *This = &lexObj;


	This->triv = (TokenTrivia) { false, false };
	while (*This->p)
	{
		while (IsWhiteSpace(*This->p))
		{
			This->triv.AfterSpace = true;
			Next(This);
		}

		if (IsIdentifier(*This->p))
		{
			ParseIdentifier(This);
			continue;
		}

		switch (*This->p)
		{
		case '\n':
			This->triv.StartOfLine = true;
			This->triv.AfterSpace = false;
			This->ln++;
			This->col = 0;
			This->disLn++;
			This->p++;
			break;
		}
	}

	return defTok.Next;
}

static const char *Next(Lexer *This)
{
	This->col++;
	return This->p++;
}

static void ParseIdentifier(Lexer *This)
{
	const char *beg = This->p;
	Next(This);
	while (IsIdentifier(*This->p))
		Next(This);

	NewLexToken(This, TK_Identifier, beg, (Evaluation) { null });
}


static void NewLexToken(Lexer *This, TokenKind kind, const char *beg, Evaluation eval)
{
	uint32_t len = This->p - beg;
	Token *tok = NewToken(kind, beg, len, This->file, This->ln, This->col - len + 1, This->disLn, This->triv, eval, null);
	This->tok->Next = tok;
	This->tok = tok;
}

Token *NewToken(TokenKind kind, const char *text, uint32_t len, const File *file, uint32_t ln, uint32_t col, uint32_t disLn, TokenTrivia trivia, Evaluation eval, Token *next)
{
	Token *This = Alloc(1, Token);
	This->Kind = kind;
	This->Text = text;
	This->Length = len;
	This->File = file;

	This->Line = ln;
	This->Column = col;
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