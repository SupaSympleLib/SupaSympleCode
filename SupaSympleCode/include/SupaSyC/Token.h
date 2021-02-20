#pragma once

typedef enum
{
	TK_EndOfFile,

	TK_Number,
	TK_Identifier,

	TK_Punctuation, // Used to mark start of punctuations
	TK_Plus = TK_Punctuation,
	TK_Minus,
	TK_Star,
	TK_Slash,

	TK_Last = TK_Slash,
} TokenKind;

static const char *const TokenKindNames[TK_Last + 1] =
{
	"EndOfFileToken",

	"NumberToken",
	"IdentifierToken",

	"PlusToken",
	"MinusToken",
	"StarToken",
	"SlashToken",
};

typedef struct
{
	bool StartOfLine : 1;
	bool AfterSpace : 1;
} TokenTrivia;

typedef union
{
	int32_t Int32Eval;
	uint32_t UInt32Eval;
} Evaluation;

typedef struct Token Token;
struct Token
{
	TokenKind Kind;
	const char *Text;
	uint32_t Length; // Length of text

	const File *File;
	uint32_t Line;
	uint32_t Column;
	uint32_t DisplayLine; // For #line
	TokenTrivia Trivia;

	Token *Next;
};

Token *NewToken(TokenKind, const char *text, uint32_t textLength, const File *file, uint32_t line, uint32_t column, uint32_t displayLine, TokenTrivia trivia, Token *next);
void DeleteToken(const Token *, bool deleteNext);

void PrintToken(const Token*);