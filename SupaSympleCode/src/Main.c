#include "SupaSyC/Common.h"

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Green);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	Token *token = Lex(file);
	printf("First Token (%p): Kind = %i, Text = %.*s", token, token->Kind, token->Length, token->Text);
	CloseFile(file);
	getchar();
}