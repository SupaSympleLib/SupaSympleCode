#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct
{
	uint32_t Number;
	const char *Name;
	const char *Source;
} File;

typedef enum
{
	TK_Identifier,
} TokenKind;

typedef struct Token Token;
struct Token
{
	TokenKind Kind;
	const char *Text;
	uint32_t Length; // Length of text

	const File *File;
	uint32_t Line;
	uint32_t DisplayLine; // For #line

	bool StartOfLine : 1;
	bool AfterSpace : 1;

	// Evaluation
	union
	{
		int32_t Int32Eval;
		uint32_t UInt32Eval;
	};
	
	Token* Next;
};

Token* Lex(const File* file);