#include "SupaSyC/Common.h"

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	CloseFile(file);
	getchar();
}