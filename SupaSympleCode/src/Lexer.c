#include "SupaSyC/Common.h"

#define IsIdentifier(c) (c > 'a' && c < 'z' || c > 'A' && c < 'Z' || c == '_')

static Token* ParseIdentifier();

Token* Lex(const File *file)
{
	const char* current = file->Source;
	char c = *current;
	if (IsIdentifier(c))
	{

	}
}