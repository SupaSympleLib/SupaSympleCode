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
static void ParseNumber(Lexer *);
static void ParseIdentifier(Lexer *);
static void ParsePunctuation(Lexer *);
static void NewLexToken(Lexer *This, TokenKind kind, const char *beg);

static bool IsDigit(char c)
{ return c >= '0' && c <= '9'; }
static bool IsIdentifier(char c)
{ return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_'; }
static bool IsWhiteSpace(char c)
{ return c == ' ' || c == '\t' || c == '\r'; }

Token *Lex(const File *file)
{
	Token defTok = { .Next = null };
	Lexer *This = &(Lexer) { file->Source, file, &defTok, (TokenTrivia) { false, false }, 1, 0, 1 };

	This->triv = (TokenTrivia) { false, false };
	while (*This->p)
	{
		while (IsWhiteSpace(*This->p))
		{
			This->triv.AfterSpace = true;
			Next(This);
		}

		if (IsDigit(*This->p))
		{
			ParseNumber(This);
			continue;
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
		case 0:
			NewLexToken(This, TK_EndOfFile, This->p++);
			goto Return;
		default:
			ParsePunctuation(This);
			break;
		}
	}

	NewLexToken(This, TK_EndOfFile, This->p++);
Return:
	return defTok.Next;
}

static const char *Next(Lexer *This)
{
	This->col++;
	return This->p++;
}

static void ParseNumber(Lexer *This)
{
	const char *beg = This->p;
	Next(This);
	while (IsDigit(*This->p))
		Next(This);

	NewLexToken(This, TK_Number, beg);
}

static void ParseIdentifier(Lexer *This)
{
	const char *beg = This->p;
	Next(This);
	while (IsIdentifier(*This->p))
		Next(This);

	NewLexToken(This, TK_Identifier, beg);
}

static void ParsePunctuation(Lexer *This)
{
	const char *const punctuations[] =
	{
		"+", "-", "*", "/", "%",
		";"
	};

	uint32_t punc = -1;
	for (uint32_t i = 0; i < sizeof(punctuations) / sizeof(*punctuations); i++)
		if (StringStartsWith(This->p, punctuations[i]))
			punc = i;
	if (punc == -1)
		ErrorAt(&(Token) { .DisplayLine = This->disLn, .Column = This->col }, "Unkown token");

	const char *beg = This->p;
	for (uint32_t i = 0; i < strlen(punctuations[punc]); i++)
		Next(This);
	NewLexToken(This, TK_Punctuation + punc, beg);
}


static void NewLexToken(Lexer *This, TokenKind kind, const char *beg)
{
	uint32_t len = This->p - beg;
	Token *tok = NewToken(kind, beg, len, This->file, This->ln, This->col - len + 1, This->disLn, This->triv, null);
	This->tok->Next = tok;
	This->tok = tok;
}

Token *NewToken(TokenKind kind, const char *text, uint32_t len, const File *file, uint32_t ln, uint32_t col, uint32_t disLn, TokenTrivia trivia, Token *next)
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

	This->Next = next;

	return This;
}

void DeleteToken(const Token *This, bool delNext)
{
	if (delNext)
		DeleteToken(This->Next, true);
	Free(This);
}


void PrintToken(const Token *This)
{
	SetConsoleColor(ConsoleColor_Green);
	printf("%-16s '%1.*s' (%u:%u)\n", TokenKindNames[This->Kind], This->Length, This->Text, This->DisplayLine, This->Column);
}