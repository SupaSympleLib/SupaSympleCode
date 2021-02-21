#include "SupaSyC/Common.h"

bool StringStartsWith(const char *str, const char *start)
{
	uint32_t strLen = strlen(str), startLen = strlen(start);
	bool sz = strLen >= startLen;
	int exact = strncmp(str, start, startLen);
	return sz && !exact;
}

__declspec(noreturn) void Error(const char *fmt, ...)
{
	SetConsoleColor(ConsoleColor_Red);
	fprintf(stderr, "[!]: \"");

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fputc('\"', stderr);
	ResetConsoleColor();
#if _DEBUG
	abort();
#else
	exit(EXIT_FAILURE);
#endif
}

__declspec(noreturn) void ErrorAt(const Token *tok, const char *fmt, ...)
{
	SetConsoleColor(ConsoleColor_Red);
	fprintf(stderr, "[!](%i:%i): \"", tok->DisplayLine, tok->Column);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fputc('\"', stderr);
	ResetConsoleColor();
#if _DEBUG
	abort();
#else
	exit(EXIT_FAILURE);
#endif
}

void *_Alloc(uint32_t count, uint32_t sz)
{
	void *ptr = calloc(count, sz);
	assert(ptr);
	return ptr;
}