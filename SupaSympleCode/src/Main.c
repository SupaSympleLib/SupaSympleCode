#include "SupaSyC/Common.h"

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Green);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	Token *token = Lex(file);
	printf("First Token (0x%p): Kind: %s (Id: 0x%x), Text: %.*s (Length: %u)", token, TokenKindNames[token->Kind], token->Kind, token->Length, token->Text, token->Length);
	CloseFile(file);
	getchar();
}