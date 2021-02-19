#include "SupaSyC/Common.h"

static void PrintTokens(Token *);

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	putchar('\n');

	SetConsoleColor(ConsoleColor_Yellow);
	puts("Tokens:");
	SetConsoleColor(ConsoleColor_Green);
	Token *token = Lex(file);
	PrintTokens(token);

	CloseFile(file);
	getchar();
}

static void PrintTokens(Token *token)
{
	while (token)
	{
		printf("%s%.*s [%s] (%u:%u)\n", token->Next ? "|--" : "L--", token->Length, token->Text, TokenKindNames[token->Kind], token->DisplayLine, token->Column);

		token = token->Next;
	}
}