#pragma once

typedef enum
{
	TK_Identifier,

	TK_Last = TK_Identifier,
} TokenKind;

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
	uint32_t DisplayLine; // For #line
	TokenTrivia Trivia;
	Evaluation Eval;

	Token *Next;
};

static const char *const TokenKindNames[TK_Last + 1] =
{
	"Identifier",
};

Token *NewToken(TokenKind, const char *text, const char *textEnd, const File *file, uint32_t line, uint32_t displayLine, TokenTrivia trivia, Evaluation, Token *next);
void DeleteToken(const Token *, bool deleteNext);