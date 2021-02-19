#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct
{
	uint32_t Number;
	const char* Name;
	const char* Source;

	// For #line directive
	const char* DisplayName;
	uint32_t DisplayLine;
} File;

typedef enum
{
	TK_Identifier,
} TokenKind;

typedef struct Token Token;
struct Token
{
	TokenKind Kind;
	union
	{
		int32_t Int32Eval;
		uint32_t UInt32Eval;
	};



	Token* Next;
};

Token* Lex(const char* source);