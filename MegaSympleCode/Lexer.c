#include "Common.h"

typedef struct
{
	const char *m_P;
} Lexer;

Token *Lex(const char *file)
{
	Lexer *this = &(Lexer) { file };

	while (*this->m_P)
	{

	}
}