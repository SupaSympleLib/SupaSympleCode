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
	printf("Tokens from '%s':\n", file->Name);
	Token *token = Lex(file);
	PrintTokens(token);

	CloseFile(file);
	getchar();
}

static void PrintTokens(Token *token)
{
	while (token)
	{
		SetConsoleColor(ConsoleColor_Cyan);
		printf("%s", token->Next ? "|--" : "L--");
		SetConsoleColor(ConsoleColor_Green);
		printf("%-10s '%1.*s' (%u:%u)\n", TokenKindNames[token->Kind], token->Length, token->Text, token->DisplayLine, token->Column);

		token = token->Next;
	}
}