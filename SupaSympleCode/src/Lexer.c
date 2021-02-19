#include "SupaSyC/Common.h"

#define IsIdentifier(c) (c > 'a' && c < 'z' || c > 'A' && c < 'Z' || c == '_')
#define IsWhiteSpace(c) (c == ' ' || c == '\t' || c == '\r' || c == 0xCC)

typedef struct
{
	const char *p;
	const File *file;
	Token *tok;
	TokenTrivia triv;

	uint32_t ln;
	uint32_t disLn;
} Lexer;

static void ParseIdentifier(Lexer *);
static void NewLexToken(Lexer *This, TokenKind kind, const char *beg, Evaluation eval);

Token *Lex(const File *file)
{
	Token defTok = {};
	Lexer lexObj = Lexer { file->Source, file, &defTok, TokenTrivia { false, false } };
	Lexer *This = &lexObj;


	This->triv = {};
	while (*This->p)
	{
		while (IsWhiteSpace(*This->p))
		{
			This->triv.AfterSpace = true;
			This->p++;
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
			This->p++;
			break;
		}
	}

	return defTok.Next;
}

static void ParseIdentifier(Lexer *This)
{
	const char* beg = This->p;
	while (IsIdentifier(*This->p++));
	
	NewLexToken(This, TK_Identifier, beg, Evaluation {});
}


static void NewLexToken(Lexer *This, TokenKind kind, const char *beg, Evaluation eval)
{
	Token *tok = NewToken(kind, beg, This->p, This->file, This->ln, This->disLn, This->triv, eval, null);
	This->tok->Next = tok;
	This->tok = tok;
}

Token *NewToken(TokenKind kind, const char *text, const char *end, const File *file, uint32_t ln, uint32_t disLn, TokenTrivia trivia, Evaluation eval, Token *next)
{
	Token* This = Alloc(1, Token);
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