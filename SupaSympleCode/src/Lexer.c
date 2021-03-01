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
static void ParseKeyword(Lexer *);
static void NewLexToken(Lexer *this, TokenKind kind, const char *beg);

static bool IsDigit(char c)
{ return c >= '0' && c <= '9' || c == '.'; }
static bool IsIdentifier(char c)
{ return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_'; }
static bool IsWhiteSpace(char c)
{ return c == ' ' || c == '\t' || c == '\r'; }

Token *Lex(const File *file)
{
	Token defTok = { .Next = null };
	Lexer *this = &(Lexer) { file->Source, file, &defTok, (TokenTrivia) { false, false }, 1, 0, 1 };

	this->triv = (TokenTrivia) { false, false };
	while (*this->p)
	{
		while (IsWhiteSpace(*this->p))
		{
			this->triv.AfterSpace = true;
			Next(this);
		}

		if (IsDigit(*this->p))
		{
			ParseNumber(this);
			continue;
		}

		if (IsIdentifier(*this->p))
		{
			ParseIdentifier(this);
			continue;
		}

		// Single line comment
		if (StringStartsWith(this->p, "//"))
		{
			// Skip two characters (Sizeof "//")
			Next(this);
			Next(this);
			while ((*this->p != '\n') && *this->p)
				Next(this);
			continue;
		}

		// Block comment
		if (StringStartsWith(this->p, "/*"))
		{
			// Skip two characters (Sizeof "/*")
			Next(this);
			Next(this);
			while (!StringStartsWith(this->p, "*/"))
			{
				if (!*Next(this))
					ErrorAt(&(Token) { .DisplayLine = this->disLn, .Column = this->col }, "Unexpected end of line");
			}
			// Skip two characters (Sizeof "*/")
			Next(this);
			Next(this);
			continue;
		}

		switch (*this->p)
		{
		case '\n':
			this->triv.StartOfLine = true;
			this->triv.AfterSpace = false;
			this->ln++;
			this->col = 0;
			this->disLn++;
			this->p++;
			break;
		case 0:
			NewLexToken(this, TK_EndOfFile, this->p++);
			goto Return;
		default:
			ParsePunctuation(this);
			break;
		}
	}

	NewLexToken(this, TK_EndOfFile, this->p++);
Return:
	return defTok.Next;
}

static const char *Next(Lexer *this)
{
	this->col++;
	return this->p++;
}

static void ParseNumber(Lexer *this)
{
	const char *beg = this->p;
	Next(this);
	while (IsDigit(*this->p))
		Next(this);

	NewLexToken(this, TK_Number, beg);
}

#define KEYWORD(key, word)                                \
	else if (!strncmp(beg, #key, this->p - beg))          \
		return NewLexToken(this, TK_##word##Keyword, beg) \

static void ParseIdentifier(Lexer *this)
{
	const char *beg = this->p;
	Next(this);
	while (IsIdentifier(*this->p))
		Next(this);

	if (false); // Test if identifier is a keyword
	KEYWORD(var, Var);
	KEYWORD(func, Func);

	else
		NewLexToken(this, TK_Identifier, beg);
}

static void ParsePunctuation(Lexer *this)
{
	const char *const punctuations[] =
	{
		"+", "-", "*", "/", "%",
		"=",
		";",
		"(", ")", "{", "}",
	};

	uint32_t punc = -1;
	for (uint32_t i = 0; i < sizeof(punctuations) / sizeof(*punctuations); i++)
		if (StringStartsWith(this->p, punctuations[i]))
			punc = i;
	if (punc == -1)
		ErrorAt(&(Token) { .DisplayLine = this->disLn, .Column = this->col }, "Unkown token");

	const char *beg = this->p;
	for (uint32_t i = 0; i < strlen(punctuations[punc]); i++)
		Next(this);
	NewLexToken(this, TK_Punctuation + punc, beg);
}


static void NewLexToken(Lexer *this, TokenKind kind, const char *beg)
{
	uint32_t len = this->p - beg;
	Token *tok = NewToken(kind, beg, len, this->file, this->ln, this->col - len + 1, this->disLn, this->triv, null);
	this->tok->Next = tok;
	this->tok = tok;
}

Token *NewToken(TokenKind kind, const char *text, uint32_t len, const File *file, uint32_t ln, uint32_t col, uint32_t disLn, TokenTrivia trivia, Token *next)
{
	Token *this = Alloc(1, Token);
	this->Kind = kind;
	this->Text = text;
	this->Length = len;
	this->File = file;

	this->Line = ln;
	this->Column = col;
	this->DisplayLine = disLn;
	this->Trivia = trivia;

	this->Next = next;

	return this;
}

void DeleteToken(const Token *this, bool delNext)
{
	if (!this)
		return;

	if (delNext)
		DeleteToken(this->Next, delNext);
	Free(this);
}


void PrintToken(const Token *this)
{
	SetConsoleColor(ConsoleColor_Green);
	printf("%-16s '%1.*s' (%u:%u)\n", TokenKindNames[this->Kind], this->Length, this->Text, this->DisplayLine, this->Column);
}